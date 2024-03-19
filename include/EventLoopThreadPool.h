#ifndef __EVENTLOOPTHREADPOOL_H__
#define __EVENTLOOPTHREADPOOL_H__

#include "EventLoopThread.h"
#include "noncopyable.h"

#include <functional>
#include <memory>
#include <string>
#include <vector>
using std::function;
using std::string;
using std::unique_ptr;
using std::vector;

class EventLoop;
class EventLoopThread;

class EventLoopThreadPool : noncopyable {
public:
    using ThreadInitCallback = function<void(EventLoop *)>;

    EventLoopThreadPool(EventLoop *baseLoop, const string &nameArg);
    ~EventLoopThreadPool();

    // 设置线程数量
    void setThreadNum(int numThreads);

    // 启动线程池创建事件循环线程
    void start(const ThreadInitCallback &cb = ThreadInitCallback());

    // 轮询获取下一个loop
    EventLoop *getNextLoop();

    // 获取所有的loop
    vector<EventLoop *> getAllLoops();

    // 获取开启状态
    bool started() const;
    // 获取名称
    string name() const;

private:
    EventLoop *baseLoop_;
    string name_;
    bool started_;
    int numThreads_;
    int next_;
    vector<unique_ptr<EventLoopThread>> threads_;
    vector<EventLoop *> loops_;
};

#endif