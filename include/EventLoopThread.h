#ifndef __EVENTLOOPTHREAD_H__
#define _ -EVENTLOOPTHREAD_H__

#include "EventLoop.h"
#include "Thread.h"
#include "noncopyable.h"

#include <condition_variable>
#include <functional>
#include <mutex>
#include <string>
using std::bind;
using std::condition_variable;
using std::function;
using std::mutex;
using std::string;
using std::unique_lock;

class EventLoop;

/*one loop per thread*/
class EventLoopThread : noncopyable {
public:
    using ThreadInitCallback = function<void(EventLoop *)>;

    EventLoopThread(const ThreadInitCallback &cb = ThreadInitCallback(),
                    const string &name = string());
    ~EventLoopThread();

    // 开启事件循环
    EventLoop *startLoop();

private:
    // 线程回调函数
    void threadFunc();

    EventLoop *loop_; // 事件循环
    bool exiting_;    // 线程关闭开关
    Thread thread_;   // 线程对象
    mutex mutex_;
    condition_variable cond_;
    ThreadInitCallback callback_; // 记录线程初始化回调函数
};

#endif