#include "../include/Thread.h"

atomic_int Thread::numCreated_(0);

Thread::Thread(ThreadFunc func, const string &name)
    : started_(false),
      join_(false),
      tid_(0),
      func_(move(func)),
      name_(name) {
    setDefaultName();
}

Thread::~Thread() {
    // 如果线程启动并且不是join等待释放就将其分离
    if (started_ && !join_) {
        thread_->detach();
    }
}

void Thread::start() {
    started_ = true;
    sem_t sem;
    sem_init(&sem, 0, 0);
    // 开启线程
    thread_ = shared_ptr<thread>(new thread([&]() {
        // 获取线程id
        tid_ = CurrentThread::tid();
        // 通知线程创建成功并获取线程id成功
        sem_post(&sem);
        // 执行线程函数
        func_();
    }));
    // 阻塞等待线程创建成功并获取线程id
    sem_wait(&sem);
}

void Thread::join() {
    join_ = true;
    thread_->join();
}

bool Thread::started() const {
    return started_;
}

pid_t Thread::tid() const {
    return tid_;
}

const string &Thread::name() const {
    return name_;
}

int Thread::numCreated() {
    return numCreated_;
}

void Thread::setDefaultName() {
    int num = ++numCreated_;
    if (name_.empty()) {
        char buf[32] = {0};
        snprintf(buf, sizeof buf, "Thread-%d", num);
        name_ = buf;
    }
}