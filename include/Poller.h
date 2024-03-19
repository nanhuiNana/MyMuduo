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

/*����IO����ģ��*/
class Poller : noncopyable {
public:
    // �Զ���ͨ���б�����
    using ChannelList = vector<Channel *>;

    Poller(EventLoop *loop);
    virtual ~Poller() = default;

    // ������IO���ñ���ͳһ�Ľӿ�
    virtual Timestamp poll(int timeoutMs, ChannelList *activeChannels) = 0;
    virtual void updateChannel(Channel *channel) = 0;
    virtual void removeChannel(Channel *channel) = 0;

    // �ж�channel�Ƿ��ڵ�ǰPoller��
    bool hasChannel(Channel *channel) const;

    // ��ȡĬ�ϵ�IO���õľ���ʵ�ֽӿ�
    static Poller *newDefaultPoller(EventLoop *loop);

protected:
    // key��socketfd value��socketfd������ͨ��
    using ChannelMap = unordered_map<int, Channel *>;
    ChannelMap channels_;

private:
    EventLoop *ownerLoop_; // ��¼Poller������EventLoop�¼�ѭ��
};

#endif