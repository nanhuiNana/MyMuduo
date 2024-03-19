#include "../include/EpollPoller.h"

const int kNew = -1;    // Channel未添加到Poller
const int kAdded = 1;   // Channel已添加到Poller
const int kDeleted = 2; // Channel从Poller已删除

EpollPoller::EpollPoller(EventLoop *loop)
    : Poller(loop),
      epollfd_(::epoll_create1(EPOLL_CLOEXEC)), // 使用epoll_create1创建一个新的epoll实例
      events_(kInitEventListSize) {
    if (epollfd_ < 0) {
        LOG_FATAL("epoll create error:%d\n", errno);
    }
}

EpollPoller::~EpollPoller() {
    ::close(epollfd_);
}

Timestamp EpollPoller::poll(int timeoutMs, ChannelList *activeChannels) {
    LOG_DEBUG("func=%s => fd total count:%lu\n", __FUNCTION__, channels_.size());

    // 调用epoll_wait
    int numEvents = ::epoll_wait(epollfd_, &*events_.begin(), static_cast<int>(events_.size()), timeoutMs);
    // 保存errno
    int saveErrno = errno;
    // 获取当前时间戳
    Timestamp now(Timestamp::now());

    // 如果有事件返回
    if (numEvents > 0) {
        LOG_INFO("%d events happened\n", numEvents);
        fillActiveChannels(numEvents, activeChannels);
        if (numEvents == static_cast<int>(events_.size())) {
            events_.resize(events_.size() * 2);
        }
    } else if (numEvents == 0) {
        LOG_DEBUG("%s timeout\n", __FUNCTION__);
    } else {
        if (saveErrno != EINTR) {
            errno = saveErrno;
            LOG_ERROR("EpollPoller:poll() error");
        }
    }
    return now;
}

void EpollPoller::updateChannel(Channel *channel) {
    const int index = channel->index();
    LOG_DEBUG("func=%s => fd=%d events=%d index=%d\n", __FUNCTION__,
              channel->fd(), channel->events(), channel->index());

    // 如果标志位位未添加或已删除
    if (index == kNew || index == kDeleted) {
        // 如果未添加
        if (index == kNew) {
            int fd = channel->fd();
            channels_[fd] = channel; // 记录fd对应通道
        }
        channel->setIndex(kAdded);      // 设置已添加标志
        update(EPOLL_CTL_ADD, channel); // 调用update更新epoll红黑树添加该通道
    } else {                            // 如果已添加
        // 如果通道当前没有任何事件
        if (channel->isNoneEvent()) {
            channel->setIndex(kDeleted);    // 设置已删除标志
            update(EPOLL_CTL_DEL, channel); // 删除该通道
        } else {
            update(EPOLL_CTL_MOD, channel); // 调整该通道
        }
    }
}
void EpollPoller::removeChannel(Channel *channel) {
    int fd = channel->fd();
    channels_.erase(fd); // 删除fd对应的键值对

    LOG_DEBUG("func=%s => fd=%d\n", __FUNCTION__, fd);

    int index = channel->index();
    // 如果已添加
    if (index == kAdded) {
        update(EPOLL_CTL_DEL, channel); // 删除该通道
    }
    channel->setIndex(kNew); // 设置标志位为未添加
}

void EpollPoller::fillActiveChannels(int numEvents, ChannelList *activeChannels) const {
    for (int i = 0; i < numEvents; i++) {
        // 从事件列表中获取通道
        Channel *channel = static_cast<Channel *>(events_[i].data.ptr);
        // 设置通道的返回事件
        channel->setRevents(events_[i].events);
        // 添加活跃通道到活跃通道列表中
        activeChannels->push_back(channel);
    }
}

void EpollPoller::update(int operation, Channel *channel) {
    epoll_event event;
    bzero(&event, sizeof event);

    int fd = channel->fd();

    // 给事件赋值
    event.data.fd = fd;
    event.events = channel->events();
    event.data.ptr = channel;

    // 调用epoll_ctl
    if (::epoll_ctl(epollfd_, operation, fd, &event) < 0) {
        if (operation == EPOLL_CTL_DEL) {
            LOG_ERROR("epoll_ctl del error:%d\n", errno);
        } else {
            LOG_FATAL("epoll_ctl add/mod error:%d\n", errno);
        }
    }
}