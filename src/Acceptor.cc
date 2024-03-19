#include "../include/Acceptor.h"

static int createNonblocking() {
    int socketfd = ::socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, 0);
    if (socketfd < 0) {
        LOG_FATAL("%s:%s:%d listen socketfd create error\n",
                  __FILE__, __FUNCTION__, __LINE__);
    }
    return socketfd;
}

Acceptor::Acceptor(EventLoop *loop, const InetAddress &listenAddr, bool reusePort)
    : loop_(loop),
      acceptSocket_(createNonblocking()),
      acceptChannel_(loop, acceptSocket_.fd()),
      listenning_(false) {
    acceptSocket_.setReuseAddr(reusePort);
    acceptSocket_.setReusePort(reusePort);
    acceptSocket_.bindAddress(listenAddr);
    acceptChannel_.setReadCallback(bind(&Acceptor::handleRead, this));
}

Acceptor::~Acceptor() {
    acceptChannel_.disableAll();
    acceptChannel_.remove();
}

void Acceptor::setNewConnectionCallback(const NewConnectionCallback &cb) {
    newConnectionCallback_ = cb;
}

bool Acceptor::listenning() const {
    return listenning_;
}

void Acceptor::listen() {
    listenning_ = true;
    acceptSocket_.listen();
    acceptChannel_.enableReading();
}

void Acceptor::handleRead() {
    InetAddress peerAddr;
    int connectfd = acceptSocket_.accept(peerAddr);
    if (connectfd >= 0) {
        if (newConnectionCallback_) {
            newConnectionCallback_(connectfd, peerAddr);
        } else {
            ::close(connectfd);
        }
    } else {
        LOG_ERROR("%s:%s:%d accept error:%d\n",
                  __FILE__, __FUNCTION__, __LINE__, errno);
        if (errno == EMFILE) {
            LOG_ERROR("%s:%s:%d socketfd reached limit!\n",
                      __FILE__, __FUNCTION__, __LINE__);
        }
    }
}