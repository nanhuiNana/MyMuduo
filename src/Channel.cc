#include "../include/Channel.h"

const int Channel::kNoneEvent = 0;
const int Channel::kReadEvent = EPOLLIN | EPOLLPRI;
const int Channel::kWriteEvent = EPOLLOUT;

Channel::Channel(EventLoop *loop, int fd)
    : loop_(loop),
      fd_(fd),
      events_(0),
      revents_(0),
      index_(-1),
      tied_(false) {
}

Channel::~Channel() {
}

void Channel::handleEvent(Timestamp receiveTime) {
    if (tied_) {
        shared_ptr<void> guard = tie_.lock();
        if (guard) {
            handleEventWithGuard(receiveTime);
        }
    } else {
        handleEventWithGuard(receiveTime);
    }
}

void Channel::setReadCallback(ReadEventCallback cb) {
    readCallback_ = move(cb);
}

void Channel::setWriteCallback(EventCallback cb) {
    writeCallback_ = move(cb);
}

void Channel::setCloseCallback(EventCallback cb) {
    closeCallback_ = move(cb);
}

void Channel::setErrorCallback(EventCallback cb) {
    errorCallback_ = move(cb);
}

void Channel::tie(const shared_ptr<void> &obj) {
    tie_ = obj;
    tied_ = true;
}

int Channel::fd() const {
    return fd_;
}

int Channel::events() const {
    return events_;
}

void Channel::setRevents(int revent) {
    revents_ = revent;
}

void Channel::enableReading() {
    events_ |= kReadEvent;
    update();
}

void Channel::disableReading() {
    events_ &= ~kReadEvent;
    update();
}

void Channel::enableWriting() {
    events_ |= kWriteEvent;
    update();
}

void Channel::disableWriting() {
    events_ &= ~kWriteEvent;
    update();
}

void Channel::disableAll() {
    events_ = kNoneEvent;
    update();
}

bool Channel::isNoneEvent() const {
    return events_ == kNoneEvent;
}

bool Channel::isReading() const {
    return events_ & kReadEvent;
}

bool Channel::isWriting() const {
    return events_ & kWriteEvent;
}

int Channel::index() {
    return index_;
}

void Channel::setIndex(int idx) {
    index_ = idx;
}

EventLoop *Channel::ownerLoop() {
    return loop_;
}

void Channel::remove() {
    loop_->removeChannel(this);
}

void Channel::update() {
    loop_->updateChannel(this);
}

void Channel::handleEventWithGuard(Timestamp receiveTime) {
    LOG_INFO("channel handleEvent revents:%d\n", revents_);

    // 判断出现挂起事件并且没有读事件
    if ((revents_ & EPOLLHUP) && !(revents_ & EPOLLIN)) {
        if (closeCallback_) {
            closeCallback_();
        }
    }
    // 判断出现错误事件
    if (revents_ & EPOLLERR) {
        if (errorCallback_) {
            errorCallback_();
        }
    }
    // 判断出现读事件或高优先级紧急读事件
    if (revents_ & (EPOLLIN | EPOLLPRI)) {
        if (readCallback_) {
            readCallback_(receiveTime);
        }
    }
    // 判断出现写事件
    if (revents_ & EPOLLOUT) {
        if (writeCallback_) {
            writeCallback_();
        }
    }
}