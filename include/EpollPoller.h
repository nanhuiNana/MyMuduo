#ifndef __EPOLLPOLLER_H__
#define __EPOLLPOLLER_H__

#include "Channel.h"
#include "Log.h"
#include "Poller.h"
#include <errno.h>
#include <strings.h>
#include <sys/epoll.h>
#include <unistd.h>

/*封装epoll具体实现类*/
class EpollPoller : public Poller {
public:
    EpollPoller(EventLoop *loop);
    ~EpollPoller() override;

    // 重写基类的抽象方法
    // 调用epoll_wait，从第二个参数中获取活跃通道
    Timestamp poll(int timeoutMs, ChannelList *activeChannels) override;
    // 更新通道
    void updateChannel(Channel *channel) override;
    // 删除通道
    void removeChannel(Channel *channel) override;

private:
    static const int kInitEventListSize = 16; //

    // 填写活跃的连接
    void fillActiveChannels(int numEvents, ChannelList *activeChannels) const;
    // 更新Channel通道
    void update(int operation, Channel *channel);

    // 自定义事件列表类型
    using EventList = vector<epoll_event>;

    int epollfd_;
    EventList events_;
};

#endif