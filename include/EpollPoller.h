#ifndef __EPOLLPOLLER_H__
#define __EPOLLPOLLER_H__

#include "Channel.h"
#include "Log.h"
#include "Poller.h"
#include <errno.h>
#include <strings.h>
#include <sys/epoll.h>
#include <unistd.h>

/*��װepoll����ʵ����*/
class EpollPoller : public Poller {
public:
    EpollPoller(EventLoop *loop);
    ~EpollPoller() override;

    // ��д����ĳ��󷽷�
    // ����epoll_wait���ӵڶ��������л�ȡ��Ծͨ��
    Timestamp poll(int timeoutMs, ChannelList *activeChannels) override;
    // ����ͨ��
    void updateChannel(Channel *channel) override;
    // ɾ��ͨ��
    void removeChannel(Channel *channel) override;

private:
    static const int kInitEventListSize = 16; //

    // ��д��Ծ������
    void fillActiveChannels(int numEvents, ChannelList *activeChannels) const;
    // ����Channelͨ��
    void update(int operation, Channel *channel);

    // �Զ����¼��б�����
    using EventList = vector<epoll_event>;

    int epollfd_;
    EventList events_;
};

#endif