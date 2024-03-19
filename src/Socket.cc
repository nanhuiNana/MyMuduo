#include "../include/Socket.h"

Socket::Socket(int socketfd)
    : socketfd_(socketfd) {
}

Socket::~Socket() {
    ::close(socketfd_);
}

int Socket::fd() const {
    return socketfd_;
}

void Socket::bindAddress(const InetAddress &localAddr) {
    if (0 != ::bind(socketfd_, (sockaddr *)localAddr.getSocketAddr(), sizeof(sockaddr_in))) {
        LOG_FATAL("%s:%s:%d bind socketfd:%d fail\n",
                  __FILE__, __FUNCTION__, __LINE__, socketfd_);
    }
}

void Socket::listen() {
    if (0 != ::listen(socketfd_, 1024)) {
        LOG_FATAL("%s:%s:%d listen socketfd:%d fail\n",
                  __FILE__, __FUNCTION__, __LINE__, socketfd_);
    }
}

int Socket::accept(InetAddress &peerAddr) {
    sockaddr_in addr;
    socklen_t len = sizeof addr;
    bzero(&addr, len);
    int connectfd = ::accept4(socketfd_, (sockaddr *)&addr, &len,
                              SOCK_NONBLOCK | SOCK_CLOEXEC);
    if (connectfd >= 0) {
        peerAddr.setSocketAddr(addr);
    }
    return connectfd;
}

void Socket::shutdownWrite() {
    if (::shutdown(socketfd_, SHUT_WR) < 0) {
        LOG_ERROR("%s:%s:%d shutdownWrite error\n",
                  __FILE__, __FUNCTION__, __LINE__);
    }
}

void Socket::setTcpNoDelay(bool on) {
    int optionValue = on ? 1 : 0;
    ::setsockopt(socketfd_, IPPROTO_TCP, TCP_NODELAY, &optionValue, sizeof optionValue);
}

void Socket::setReuseAddr(bool on) {
    int optionValue = on ? 1 : 0;
    ::setsockopt(socketfd_, SOL_SOCKET, SO_REUSEADDR, &optionValue, sizeof optionValue);
}

void Socket::setReusePort(bool on) {
    int optionValue = on ? 1 : 0;
    ::setsockopt(socketfd_, SOL_SOCKET, SO_REUSEPORT, &optionValue, sizeof optionValue);
}

void Socket::setKeepAlive(bool on) {
    int optionValue = on ? 1 : 0;
    ::setsockopt(socketfd_, SOL_SOCKET, SO_KEEPALIVE, &optionValue, sizeof optionValue);
}