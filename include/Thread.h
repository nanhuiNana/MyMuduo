#ifndef __THREAD_H__
#define __THREAD_H__

#include "CurrentThread.h"
#include "noncopyable.h"

#include <atomic>
#include <functional>
#include <memory>
#include <semaphore.h>
#include <string>
#include <thread>
#include <unistd.h>
using std::atomic_int;
using std::function;
using std::move;
using std::shared_ptr;
using std::string;
using std::thread;

/*封装线程操作*/
class Thread : noncopyable {
public:
    using ThreadFunc = function<void()>;

    explicit Thread(ThreadFunc, const string &name = string());
    ~Thread();

    // 启动线程
    void start();
    // 等待线程
    void join();

    // 获取线程启动状态
    bool started() const;
    // 获取线程id
    pid_t tid() const;
    // 获取线程名称
    const string &name() const;

    // 获取线程总数
    static int numCreated();

private:
    // 设置线程的默认名字
    void setDefaultName();

    bool started_;                 // 启动开关
    bool join_;                    // 等待开关
    shared_ptr<thread> thread_;    // 使用智能制造管理线程
    pid_t tid_;                    // 记录线程id
    ThreadFunc func_;              // 记录线程函数
    string name_;                  // 线程名称
    static atomic_int numCreated_; // 线程总数量
};

#endif