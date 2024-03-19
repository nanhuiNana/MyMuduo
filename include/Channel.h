#ifndef __CHANNEL_H__
#define __CHANNEL_H__

#include "EventLoop.h"
#include "Log.h"
#include "Timestamp.h"
#include "noncopyable.h"

#include <functional>
#include <memory>
#include <sys/epoll.h>
using std::function;
using std::move;
using std::shared_ptr;
using std::weak_ptr;

class EventLoop;

/*ͨ������װ��socketfd����events�¼�*/
class Channel : noncopyable {
public:
    // �Զ����¼��ص�����
    using EventCallback = function<void()>;
    // �Զ�����¼��ص�����
    using ReadEventCallback = function<void(Timestamp)>;

    // ���캯������EventLoop�����fd
    Channel(EventLoop *loop, int fd);
    ~Channel();

    // �����¼�
    void handleEvent(Timestamp receiveTime);

    // ���ûص�����
    void setReadCallback(ReadEventCallback cb);
    void setWriteCallback(EventCallback cb);
    void setCloseCallback(EventCallback cb);
    void setErrorCallback(EventCallback cb);

    void tie(const shared_ptr<void> &);

    // ��ȡfd�׽���
    int fd() const;
    // ��ȡeventsע���¼�
    int events() const;
    // ����revents�����¼�
    void setRevents(int revent);

    // ����fd���¼�״̬
    void enableReading();
    void disableReading();
    void enableWriting();
    void disableWriting();
    void disableAll();

    // �ж�fd��ǰ���¼�״̬
    bool isNoneEvent() const;
    bool isReading() const;
    bool isWriting() const;

    // ��ȡ��־λ
    int index();
    // ���ñ�־λ
    void setIndex(int idx);

    // ��ȡ��Channel��Ӧ��EventLoop���
    EventLoop *ownerLoop();
    // �Ƴ�����
    void remove();

private:
    // �����¼�
    void update();
    // �����¼�������Ӧ�Ļص�����
    void handleEventWithGuard(Timestamp receiveTime);

    // �Զ����¼�״̬
    static const int kNoneEvent;
    static const int kReadEvent;
    static const int kWriteEvent;

    EventLoop *loop_; // �¼�ѭ�����
    const int fd_;    // �׽���
    int events_;      // ע����¼�
    int revents_;     // ���ص��¼�
    int index_;       // ��־λ

    // add note
    weak_ptr<void> tie_;
    bool tied_;

    // ��¼��Ӧ�Ļص�����
    ReadEventCallback readCallback_;
    EventCallback writeCallback_;
    EventCallback closeCallback_;
    EventCallback errorCallback_;
};

#endif