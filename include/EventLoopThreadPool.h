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

    // �����߳�����
    void setThreadNum(int numThreads);

    // �����̳߳ش����¼�ѭ���߳�
    void start(const ThreadInitCallback &cb = ThreadInitCallback());

    // ��ѯ��ȡ��һ��loop
    EventLoop *getNextLoop();

    // ��ȡ���е�loop
    vector<EventLoop *> getAllLoops();

    // ��ȡ����״̬
    bool started() const;
    // ��ȡ����
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