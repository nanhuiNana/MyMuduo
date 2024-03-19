#include "../include/TcpServer.h"

static EventLoop *checkLoopNotNull(EventLoop *loop) {
    if (loop == nullptr) {
        LOG_FATAL("%s:%s:%d mainloop is null\n",
                  __FILE__, __FUNCTION__, __LINE__);
    }
    return loop;
}

TcpServer::TcpServer(EventLoop *loop, const InetAddress &listenAddr,
                     const string &name, OPTION option)
    : loop_(checkLoopNotNull(loop)),
      ipPort_(listenAddr.toIpPort()),
      name_(name),
      acceptor_(new Acceptor(loop, listenAddr, option == kReusePort)),
      threadPool_(new EventLoopThreadPool(loop, name_)),
      connectionCallback_(),
      messageCallback_(),
      started_(0),
      nextConnId_(1) {
    acceptor_->setNewConnectionCallback(bind(&TcpServer::newConnection, this, _1, _2));
}

TcpServer::~TcpServer() {
    for (auto &item : connections_) {
        TcpConnectionPtr conn(item.second);
        item.second.reset();

        conn->getLoop()->runInLoop(bind(&TcpConnection::connectDestroyed, conn));
    }
}

void TcpServer::setThreadInitCallback(const ThreadInitCallback &cb) {
    threadInitCallback_ = cb;
}

void TcpServer::setConnectionCallback(const ConnectionCallback &cb) {
    connectionCallback_ = cb;
}

void TcpServer::setMessageCallback(const MessageCallback &cb) {
    messageCallback_ = cb;
}

void TcpServer::setWriteCompleteCallback(const WriteCompleteCallback &cb) {
    writeCompleteCallback_ = cb;
}

void TcpServer::setThreadNum(int numThreads) {
    threadPool_->setThreadNum(numThreads);
}

void TcpServer::start() {
    if (started_++ == 0) {
        threadPool_->start(threadInitCallback_);
        loop_->runInLoop(bind(&Acceptor::listen, acceptor_.get()));
    }
}

void TcpServer::newConnection(int socketfd, const InetAddress &peerAddr) {
    EventLoop *ioLoop = threadPool_->getNextLoop();
    char buf[64] = {0};
    snprintf(buf, sizeof buf, "-%s#%d", ipPort_.c_str(), nextConnId_);
    ++nextConnId_;
    string connName = name_ + buf;

    LOG_INFO("TcpServer::newConnection [%s] - new connection [%s] from %s\n",
             name_.c_str(), connName.c_str(), peerAddr.toIpPort().c_str());

    sockaddr_in local;
    ::bzero(&local, sizeof local);
    socklen_t addrlen = sizeof local;
    if (::getsockname(socketfd, (sockaddr *)&local, &addrlen) < 0) {
        LOG_ERROR("%s:%s:%d\n", __FILE__, __FUNCTION__, __LINE__);
    }
    InetAddress localAddr(local);

    TcpConnectionPtr conn(new TcpConnection(ioLoop, connName, socketfd, localAddr, peerAddr));
    connections_[connName] = conn;
    conn->setConnectionCallback(connectionCallback_);
    conn->setMessageCallback(messageCallback_);
    conn->setWriteCompleteCallback(writeCompleteCallback_);

    conn->setCloseCallback(bind(&TcpServer::removeConnection, this, _1));

    ioLoop->runInLoop(bind(&TcpConnection::connectEstablished, conn));
}

void TcpServer::removeConnection(const TcpConnectionPtr &conn) {
    loop_->runInLoop(bind(&TcpServer::removeConnectionInLoop, this, conn));
}

void TcpServer::removeConnectionInLoop(const TcpConnectionPtr &conn) {
    LOG_INFO("TcpServer::removeConnectionInLoop [%s] - connection %s\n", name_.c_str(), conn->name().c_str());
    connections_.erase(conn->name());
    EventLoop *ioLoop = conn->getLoop();
    ioLoop->queueInLoop(bind(&TcpConnection::connectDestroyed, conn));
}