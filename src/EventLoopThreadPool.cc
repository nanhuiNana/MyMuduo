#include "../include/EventLoopThreadPool.h"

EventLoopThreadPool::EventLoopThreadPool(EventLoop *baseLoop, const string &nameArg)
    : baseLoop_(baseLoop),
      name_(nameArg),
      started_(false),
      numThreads_(0),
      next_(0) {
}

EventLoopThreadPool::~EventLoopThreadPool() {
}

void EventLoopThreadPool::setThreadNum(int numThreads) {
    numThreads_ = numThreads;
}

void EventLoopThreadPool::start(const ThreadInitCallback &cb) {
    started_ = true;

    for (int i = 0; i < numThreads_; ++i) {
        char buf[name_.size() + 32];
        snprintf(buf, sizeof buf, "%s-%d", name_.c_str(), i);
        EventLoopThread *t = new EventLoopThread(cb, buf);
        threads_.push_back(unique_ptr<EventLoopThread>(t));
        loops_.push_back(t->startLoop());
    }

    if (numThreads_ == 0 && cb) {
        cb(baseLoop_);
    }
}

EventLoop *EventLoopThreadPool::getNextLoop() {
    EventLoop *loop = baseLoop_;
    // LOG_INFO("loops num is %d", loops_.size());
    if (!loops_.empty()) {
        loop = loops_[next_];
        ++next_;
        if (static_cast<size_t>(next_) >= loops_.size()) {
            next_ = 0;
        }
    }
    return loop;
}

vector<EventLoop *> EventLoopThreadPool::getAllLoops() {
    if (loops_.empty()) {
        return vector<EventLoop *>(1, baseLoop_);
    } else {
        return loops_;
    }
}

bool EventLoopThreadPool::started() const {
    return started_;
}

string EventLoopThreadPool::name() const {
    return name_;
}