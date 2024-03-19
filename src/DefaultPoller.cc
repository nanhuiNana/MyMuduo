#include "../include/EpollPoller.h"
#include "../include/Poller.h"

#include <stdlib.h>

Poller *Poller::newDefaultPoller(EventLoop *loop) {
    if (::getenv("MUDUO_USE_POLL")) {
        return nullptr; // ����poll��ʵ��
    } else {
        return new EpollPoller(loop); // ����epoll��ʵ��
    }
}