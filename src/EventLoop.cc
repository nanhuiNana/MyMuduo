#include "../include/EventLoop.h"

// ��ֹһ���̴߳������EventLoop
__thread EventLoop *t_loopInThisThread = nullptr;

// ����Ĭ�ϳ�ʱʱ��
const int kPollTimeMs = 10000;

// ����weakUpfd
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

    // ����wakeUpChannel���¼������Լ������¼���Ļص�������ÿһ��EventLoop��������wakeUpChannel�Ķ��¼�
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
        // ����Poller��poll��ȡ��Ծͨ���б�
        pollReturnTime_ = poller_->poll(kPollTimeMs, &activeChannels_);
        for (Channel *channel : activeChannels_) {
            // ����ͨ���Ĵ����¼�����
            channel->handleEvent(pollReturnTime_);
        }
        // ִ�е�ǰEventLoop�¼�ѭ����Ҫ����Ļص�����
        doPendingFunctors();
    }
    LOG_INFO("EventLoop %p stop looping\n", this);
    looping_ = false;
}

void EventLoop::quit() {
    quit_ = false;
    if (!isInLoopThread()) { // �����loop�̵߳�����Ҫ֪ͨ����loop�߳�
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
        unique_lock<mutex> lock(mutex_);   // ��������֤�̰߳�ȫ
        PendingFunctors_.emplace_back(cb); // ����ص���������
    }
    // �жϷ�loop�̻߳��ߵ�ǰloop�߳�����ִ�лص�
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
        // ���ص��������н������ֲ�������
        unique_lock<mutex> lock(mutex_);
        functors.swap(PendingFunctors_);
    }

    // ִ�лص�
    for (const Functor &functor : functors) {
        functor();
    }
    callingPendingFunctors_ = false;
}