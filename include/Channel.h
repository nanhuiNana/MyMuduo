#ifndef __CHANNEL_H__
#define __CHANNEL_H__

#include "EventLoop.h"
#include "Log.h"
#include "Timestamp.h"
#include "noncopyable.h"

#include <functional>
#include <memory>
#include <sys/epoll.h>
using std::function;
using std::move;
using std::shared_ptr;
using std::weak_ptr;

class EventLoop;

/*通道，封装了socketfd和其events事件*/
class Channel : noncopyable {
public:
    // 自定义事件回调类型
    using EventCallback = function<void()>;
    // 自定义读事件回调类型
    using ReadEventCallback = function<void(Timestamp)>;

    // 构造函数设置EventLoop句柄和fd
    Channel(EventLoop *loop, int fd);
    ~Channel();

    // 处理事件
    void handleEvent(Timestamp receiveTime);

    // 设置回调函数
    void setReadCallback(ReadEventCallback cb);
    void setWriteCallback(EventCallback cb);
    void setCloseCallback(EventCallback cb);
    void setErrorCallback(EventCallback cb);

    void tie(const shared_ptr<void> &);

    // 获取fd套接字
    int fd() const;
    // 获取events注册事件
    int events() const;
    // 设置revents返回事件
    void setRevents(int revent);

    // 设置fd的事件状态
    void enableReading();
    void disableReading();
    void enableWriting();
    void disableWriting();
    void disableAll();

    // 判断fd当前的事件状态
    bool isNoneEvent() const;
    bool isReading() const;
    bool isWriting() const;

    // 获取标志位
    int index();
    // 设置标志位
    void setIndex(int idx);

    // 获取该Channel对应的EventLoop句柄
    EventLoop *ownerLoop();
    // 移除自身
    void remove();

private:
    // 更新事件
    void update();
    // 处理事件调用相应的回调函数
    void handleEventWithGuard(Timestamp receiveTime);

    // 自定义事件状态
    static const int kNoneEvent;
    static const int kReadEvent;
    static const int kWriteEvent;

    EventLoop *loop_; // 事件循环句柄
    const int fd_;    // 套接字
    int events_;      // 注册的事件
    int revents_;     // 返回的事件
    int index_;       // 标志位

    // add note
    weak_ptr<void> tie_;
    bool tied_;

    // 记录相应的回调函数
    ReadEventCallback readCallback_;
    EventCallback writeCallback_;
    EventCallback closeCallback_;
    EventCallback errorCallback_;
};

#endif