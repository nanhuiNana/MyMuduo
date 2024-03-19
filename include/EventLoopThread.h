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

    // �����¼�ѭ��
    EventLoop *startLoop();

private:
    // �̻߳ص�����
    void threadFunc();

    EventLoop *loop_; // �¼�ѭ��
    bool exiting_;    // �̹߳رտ���
    Thread thread_;   // �̶߳���
    mutex mutex_;
    condition_variable cond_;
    ThreadInitCallback callback_; // ��¼�̳߳�ʼ���ص�����
};

#endif