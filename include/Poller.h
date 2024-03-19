#ifndef __POLLER_H__
#define __POLLER_H__

#include "Channel.h"
#include "Timestamp.h"
#include "noncopyable.h"
#include <unordered_map>
#include <vector>
using std::unordered_map;
using std::vector;

class Channel;
class EventLoop;

/*核心IO复用模块*/
class Poller : noncopyable {
public:
    // 自定义通道列表类型
    using ChannelList = vector<Channel *>;

    Poller(EventLoop *loop);
    virtual ~Poller() = default;

    // 给所有IO复用保留统一的接口
    virtual Timestamp poll(int timeoutMs, ChannelList *activeChannels) = 0;
    virtual void updateChannel(Channel *channel) = 0;
    virtual void removeChannel(Channel *channel) = 0;

    // 判断channel是否在当前Poller中
    bool hasChannel(Channel *channel) const;

    // 获取默认的IO复用的具体实现接口
    static Poller *newDefaultPoller(EventLoop *loop);

protected:
    // key：socketfd value：socketfd所属的通道
    using ChannelMap = unordered_map<int, Channel *>;
    ChannelMap channels_;

private:
    EventLoop *ownerLoop_; // 记录Poller所属的EventLoop事件循环
};

#endif