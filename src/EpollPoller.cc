#include "../include/EpollPoller.h"

const int kNew = -1;    // Channelδ��ӵ�Poller
const int kAdded = 1;   // Channel����ӵ�Poller
const int kDeleted = 2; // Channel��Poller��ɾ��

EpollPoller::EpollPoller(EventLoop *loop)
    : Poller(loop),
      epollfd_(::epoll_create1(EPOLL_CLOEXEC)), // ʹ��epoll_create1����һ���µ�epollʵ��
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

    // ����epoll_wait
    int numEvents = ::epoll_wait(epollfd_, &*events_.begin(), static_cast<int>(events_.size()), timeoutMs);
    // ����errno
    int saveErrno = errno;
    // ��ȡ��ǰʱ���
    Timestamp now(Timestamp::now());

    // ������¼�����
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

    // �����־λλδ��ӻ���ɾ��
    if (index == kNew || index == kDeleted) {
        // ���δ���
        if (index == kNew) {
            int fd = channel->fd();
            channels_[fd] = channel; // ��¼fd��Ӧͨ��
        }
        channel->setIndex(kAdded);      // ��������ӱ�־
        update(EPOLL_CTL_ADD, channel); // ����update����epoll�������Ӹ�ͨ��
    } else {                            // ��������
        // ���ͨ����ǰû���κ��¼�
        if (channel->isNoneEvent()) {
            channel->setIndex(kDeleted);    // ������ɾ����־
            update(EPOLL_CTL_DEL, channel); // ɾ����ͨ��
        } else {
            update(EPOLL_CTL_MOD, channel); // ������ͨ��
        }
    }
}
void EpollPoller::removeChannel(Channel *channel) {
    int fd = channel->fd();
    channels_.erase(fd); // ɾ��fd��Ӧ�ļ�ֵ��

    LOG_DEBUG("func=%s => fd=%d\n", __FUNCTION__, fd);

    int index = channel->index();
    // ��������
    if (index == kAdded) {
        update(EPOLL_CTL_DEL, channel); // ɾ����ͨ��
    }
    channel->setIndex(kNew); // ���ñ�־λΪδ���
}

void EpollPoller::fillActiveChannels(int numEvents, ChannelList *activeChannels) const {
    for (int i = 0; i < numEvents; i++) {
        // ���¼��б��л�ȡͨ��
        Channel *channel = static_cast<Channel *>(events_[i].data.ptr);
        // ����ͨ���ķ����¼�
        channel->setRevents(events_[i].events);
        // ��ӻ�Ծͨ������Ծͨ���б���
        activeChannels->push_back(channel);
    }
}

void EpollPoller::update(int operation, Channel *channel) {
    epoll_event event;
    bzero(&event, sizeof event);

    int fd = channel->fd();

    // ���¼���ֵ
    event.data.fd = fd;
    event.events = channel->events();
    event.data.ptr = channel;

    // ����epoll_ctl
    if (::epoll_ctl(epollfd_, operation, fd, &event) < 0) {
        if (operation == EPOLL_CTL_DEL) {
            LOG_ERROR("epoll_ctl del error:%d\n", errno);
        } else {
            LOG_FATAL("epoll_ctl add/mod error:%d\n", errno);
        }
    }
}