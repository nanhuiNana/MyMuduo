#ifndef __EVENTLOOP_H__
#define __EVENTLOOP_H__

#include "Channel.h"
#include "CurrentThread.h"
#include "Log.h"
#include "Poller.h"
#include "Timestamp.h"
#include "noncopyable.h"

#include <atomic>
#include <errno.h>
#include <fcntl.h>
#include <functional>
#include <memory>
#include <mutex>
#include <sys/eventfd.h>
#include <unistd.h>
#include <vector>
using std::atomic_bool;
using std::bind;
using std::function;
using std::mutex;
using std::unique_lock;
using std::unique_ptr;
using std::vector;

class Channel;
class Poller;

/*事件循环类*/
class EventLoop : noncopyable {
public:
    using Functor = function<void()>;

    EventLoop();
    ~EventLoop();

    // 开启事件循环
    void loop();
    // 退出事件循环
    void quit();

    // 获取Poller返回时间戳
    Timestamp pollReturnTime() const;

    // 在当前loop中执行cb
    void runInLoop(Functor cb);
    // 将cb放入队列，唤醒loop所在线程执行cb
    void queueInLoop(Functor cb);

    // 唤醒loop所在线程，本质上往wakeUpfd写
    void wakeUp();

    // 调用Poller的相应函数
    void updateChannel(Channel *channel);
    void removeChannel(Channel *channel);
    bool hasChannel(Channel *channel);

    // 判断EventLoop对象是否在自己的线程里
    bool isInLoopThread() const;

private:
    // wakeUpChannel回调函数，即从wakeUpfd读
    void handleRead();
    // 执行回调
    void doPendingFunctors();

    atomic_bool looping_; // 标识开启事件循环
    atomic_bool quit_;    // 标识退出事件循环

    const pid_t threadId_; // 记录当前loop所在的线程的ID

    Timestamp pollReturnTime_;  // 记录Poller返回发生事件的Channels的时间点
    unique_ptr<Poller> poller_; // Poller句柄

    int wakeUpfd_; // 用于统一事件源，唤醒loop线程也采用监听套接字读写事件进行
    unique_ptr<Channel> wakeUpChannel_;

    using ChannelList = vector<Channel *>;
    ChannelList activeChannels_; // 活跃通道列表

    atomic_bool callingPendingFunctors_; // 标识当前loop是否有需要执行的回调操作
    vector<Functor> PendingFunctors_;    // 存储当前loop需要执行的所有的回调操作
    mutex mutex_;                        // 用于保护上面vector线程安全的互斥锁
};

#endif