#include "../include/TcpConnection.h"

static EventLoop *checkLoopNotNull(EventLoop *loop) {
    if (loop == nullptr) {
        LOG_FATAL("%s:%s:%d TcpConnection Loop is Null\n",
                  __FILE__, __FUNCTION__, __LINE__);
    }
    return loop;
}

TcpConnection::TcpConnection(EventLoop *loop, const string name, int socketfd,
                             const InetAddress &localAddr, const InetAddress &peerAddr)
    : loop_(checkLoopNotNull(loop)),
      name_(name),
      state_(kConnecting),
      reading_(true),
      socket_(new Socket(socketfd)),
      channel_(new Channel(loop, socketfd)),
      localAddress_(localAddr),
      peerAddress_(peerAddr),
      highWaterMark_(64 * 1024 * 1024) // 64M
{
    channel_->setReadCallback(bind(&TcpConnection::handleRead, this, _1));
    channel_->setWriteCallback(bind(&TcpConnection::handlelWrite, this));
    channel_->setCloseCallback(bind(&TcpConnection::handleClose, this));
    channel_->setErrorCallback(bind(&TcpConnection::handleError, this));
    LOG_INFO("%s:%s:%d TcpConnection ctor[%s] at fd=%d\n",
             __FILE__, __FUNCTION__, __LINE__, name.c_str(), socketfd);
    socket_->setKeepAlive(true);
}

TcpConnection::~TcpConnection() {
    LOG_INFO("%s:%s:%d TcpConnection dtor[%s] at fd=%d state=%d\n",
             __FILE__, __FUNCTION__, __LINE__, name_.c_str(), channel_->fd(), (int)state_);
}

EventLoop *TcpConnection::getLoop() const {
    return loop_;
}

const string &TcpConnection::name() const {
    return name_;
}

const InetAddress &TcpConnection::localAddress() const {
    return localAddress_;
}

const InetAddress &TcpConnection::peerAddress() const {
    return peerAddress_;
}

bool TcpConnection::connected() const {
    return state_ == kConnected;
}

void TcpConnection::send(const string &buf) {
    if (state_ == kConnected) {
        if (loop_->isInLoopThread()) {
            sendInLoop(buf.c_str(), buf.size());
        } else {
            loop_->runInLoop(bind(&TcpConnection::sendInLoop, this, buf.c_str(), buf.size()));
        }
    }
}

void TcpConnection::shutdown() {
    if (state_ == kConnected) {
        setState(kDisconnecting);
        loop_->runInLoop(bind(&TcpConnection::shutdownInLoop, this));
    }
}

void TcpConnection::setConnectionCallback(const ConnectionCallback &cb) {
    connectionCallback_ = cb;
}

void TcpConnection::setMessageCallback(const MessageCallback &cb) {
    messageCallback_ = cb;
}

void TcpConnection::setWriteCompleteCallback(const WriteCompleteCallback &cb) {
    writeCompleteCallback_ = cb;
}

void TcpConnection::setHighWaterMarkCallback(const HighWaterMarkCallback &cb, size_t highWaterMark) {
    highWaterMarkCallback_ = cb;
    highWaterMark_ = highWaterMark;
}

void TcpConnection::setCloseCallback(const CloseCallback &cb) {
    closeCallback_ = cb;
}

void TcpConnection::connectEstablished() {
    setState(kConnected);
    channel_->tie(shared_from_this()); // 调用tie来绑定自身，防止channel还在执行handleEvent时自身结束析构掉了channel导致异常
    channel_->enableReading();

    connectionCallback_(shared_from_this());
}

void TcpConnection::connectDestroyed() {
    if (state_ == kConnected) {
        setState(kDisconnected);
        channel_->disableAll();
        connectionCallback_(shared_from_this());
    }
    channel_->remove();
}

void TcpConnection::setState(STATE state) {
    state_ = state;
}

void TcpConnection::handleRead(Timestamp receiveTime) {
    int saveErrno = 0;
    ssize_t n = inputBuffer_.readFd(channel_->fd(), &saveErrno);
    if (n > 0) {
        messageCallback_(shared_from_this(), &inputBuffer_, receiveTime);
    } else if (n == 0) {
        handleClose();
    } else {
        errno = saveErrno;
        LOG_ERROR("%s:%s:%d \n",
                  __FILE__, __FUNCTION__, __LINE__);
        handleError();
    }
}

void TcpConnection::handlelWrite() {
    if (channel_->isWriting()) {
        int saveErrno = 0;
        ssize_t n = outputBuffer_.writeFd(channel_->fd(), &saveErrno);
        if (n > 0) {
            outputBuffer_.retrieve(n);
            if (outputBuffer_.readableBytes() == 0) {
                channel_->disableWriting();
                if (writeCompleteCallback_) {
                    loop_->queueInLoop(bind(writeCompleteCallback_, shared_from_this()));
                }
                if (state_ == kDisconnecting) {
                    shutdownInLoop();
                }
            }
        } else {
            LOG_ERROR("%s:%s:%d \n",
                      __FILE__, __FUNCTION__, __LINE__);
        }
    } else {
        LOG_ERROR("%s:%s:%d TcpConnection fd=%d is down, no more writing\n",
                  __FILE__, __FUNCTION__, __LINE__, channel_->fd());
    }
}

void TcpConnection::handleClose() {
    LOG_INFO("%s:%s:%d fd=%d state=%d\n",
             __FILE__, __FUNCTION__, __LINE__, channel_->fd(), (int)state_);
    setState(kDisconnected);
    channel_->disableAll();

    TcpConnectionPtr connPtr = shared_from_this();
    connectionCallback_(connPtr);
    closeCallback_(connPtr);
}

void TcpConnection::handleError() {
    int optionValue;
    socklen_t optionValueLen = sizeof optionValue;
    int err = 0;
    if (::getsockopt(channel_->fd(), SOL_SOCKET, SO_ERROR, &optionValue, &optionValueLen) < 0) {
        err = errno;
    } else {
        err = optionValue;
    }
    LOG_ERROR("%s:%s:%d name=%s so_error=%d\n",
              __FILE__, __FUNCTION__, __LINE__, name_.c_str(), err);
}

void TcpConnection::sendInLoop(const void *message, size_t len) {
    ssize_t nwrote = 0;
    size_t remaining = len;
    bool faultError = false;

    if (state_ == kDisconnected) {
        LOG_ERROR("%s:%s:%d diconnected, give up writing\n",
                  __FILE__, __FUNCTION__, __LINE__);
        return;
    }

    if (!channel_->isWriting() && outputBuffer_.readableBytes() == 0) {
        nwrote = ::write(channel_->fd(), message, len);
        if (nwrote >= 0) {
            remaining = len - nwrote;
            if (remaining == 0 && writeCompleteCallback_) {
                loop_->queueInLoop(bind(writeCompleteCallback_, shared_from_this()));
            }
        } else {
            nwrote = 0;
            if (errno != EWOULDBLOCK) {
                LOG_ERROR("%s:%s:%d",
                          __FILE__, __FUNCTION__, __LINE__);
                if (errno == EPIPE || errno == ECONNRESET) {
                    faultError = true;
                }
            }
        }
    }

    if (!faultError && remaining > 0) {
        size_t oldLen = outputBuffer_.readableBytes();
        if (oldLen + remaining >= highWaterMark_ && oldLen < highWaterMark_ && highWaterMarkCallback_) {
            loop_->queueInLoop(bind(highWaterMarkCallback_, shared_from_this(), oldLen + remaining));
        }
        outputBuffer_.append((char *)message + nwrote, remaining);
        if (!channel_->isWriting()) {
            channel_->enableWriting();
        }
    }
}

void TcpConnection::shutdownInLoop() {
    if (!channel_->isWriting()) {
        socket_->shutdownWrite();
    }
}