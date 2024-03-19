#include "../include/EventLoop.h"

// 防止一个线程创建多个EventLoop
__thread EventLoop *t_loopInThisThread = nullptr;

// 定义默认超时时间
const int kPollTimeMs = 10000;

// 创建weakUpfd
int createEventfd() {
    int eventFd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    if (eventFd < 0) {
        LOG_FATAL("eventfd error:%d\n", errno);
    }
    return eventFd;
}

EventLoop::EventLoop()
    : looping_(false),
      quit_(false),
      threadId_(CurrentThread::tid()),
      poller_(Poller::newDefaultPoller(this)),
      wakeUpfd_(createEventfd()),
      wakeUpChannel_(new Channel(this, wakeUpfd_)),
      callingPendingFunctors_(false) {
    LOG_DEBUG("EventLoop created %p in thread %d\n", this, threadId_);
    if (t_loopInThisThread) {
        LOG_FATAL("Another EventLoop %p exists in this thread %d\n", t_loopInThisThread, threadId_);
    } else {
        t_loopInThisThread = this;
    }

    // 设置wakeUpChannel的事件类型以及发生事件后的回调函数，每一个EventLoop都将监听wakeUpChannel的读事件
    wakeUpChannel_->setReadCallback(bind(&EventLoop::handleRead, this));
    wakeUpChannel_->enableReading();
}

EventLoop::~EventLoop() {
    wakeUpChannel_->disableAll();
    wakeUpChannel_->remove();
    ::close(wakeUpfd_);
    t_loopInThisThread = nullptr;
}

void EventLoop::loop() {
    looping_ = true;
    quit_ = false;

    LOG_INFO("EventLoop %p start looping\n", this);

    while (!quit_) {
        activeChannels_.clear();
        // 调用Poller的poll获取活跃通道列表
        pollReturnTime_ = poller_->poll(kPollTimeMs, &activeChannels_);
        for (Channel *channel : activeChannels_) {
            // 调用通道的处理事件函数
            channel->handleEvent(pollReturnTime_);
        }
        // 执行当前EventLoop事件循环需要处理的回调操作
        doPendingFunctors();
    }
    LOG_INFO("EventLoop %p stop looping\n", this);
    looping_ = false;
}

void EventLoop::quit() {
    quit_ = false;
    if (!isInLoopThread()) { // 如果非loop线程调用需要通知唤醒loop线程
        wakeUp();
    }
}

Timestamp EventLoop::pollReturnTime() const {
    return pollReturnTime_;
}

void EventLoop::runInLoop(Functor cb) {
    if (isInLoopThread()) {
        cb();
    } else {
        queueInLoop(cb);
    }
}

void EventLoop::queueInLoop(Functor cb) {
    {
        unique_lock<mutex> lock(mutex_);   // 上锁，保证线程安全
        PendingFunctors_.emplace_back(cb); // 加入回调函数队列
    }
    // 判断非loop线程或者当前loop线程正在执行回调
    if (!isInLoopThread() || callingPendingFunctors_) {
        wakeUp();
    }
}

void EventLoop::handleRead() {
    uint64_t one = 1;
    ssize_t n = read(wakeUpfd_, &one, sizeof one);
    if (n != sizeof one) {
        LOG_ERROR("EventLoop::handleRead() read %lu bytes instead of 8\n", n);
    }
}

void EventLoop::wakeUp() {
    uint64_t one = 1;
    ssize_t n = write(wakeUpfd_, &one, sizeof one);
    if (n != sizeof one) {
        LOG_ERROR("EventLoop::wakeUp() write %lu bytes instead of 8\n", n);
    }
}

void EventLoop::updateChannel(Channel *channel) {
    poller_->updateChannel(channel);
}

void EventLoop::removeChannel(Channel *channel) {
    poller_->removeChannel(channel);
}

bool EventLoop::hasChannel(Channel *channel) {
    return poller_->hasChannel(channel);
}

bool EventLoop::isInLoopThread() const {
    return threadId_ == CurrentThread::tid();
}

void EventLoop::doPendingFunctors() {
    vector<Functor> functors;
    callingPendingFunctors_ = true;
    {
        // 将回调函数队列交换到局部队列中
        unique_lock<mutex> lock(mutex_);
        functors.swap(PendingFunctors_);
    }

    // 执行回调
    for (const Functor &functor : functors) {
        functor();
    }
    callingPendingFunctors_ = false;
}