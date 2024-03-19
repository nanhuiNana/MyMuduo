#ifndef __EVENTLOOP_H__
#define __EVENTLOOP_H__

#include "Channel.h"
#include "CurrentThread.h"
#include "Log.h"
#include "Poller.h"
#include "Timestamp.h"
#include "noncopyable.h"

#include <atomic>
#include <errno.h>
#include <fcntl.h>
#include <functional>
#include <memory>
#include <mutex>
#include <sys/eventfd.h>
#include <unistd.h>
#include <vector>
using std::atomic_bool;
using std::bind;
using std::function;
using std::mutex;
using std::unique_lock;
using std::unique_ptr;
using std::vector;

class Channel;
class Poller;

/*�¼�ѭ����*/
class EventLoop : noncopyable {
public:
    using Functor = function<void()>;

    EventLoop();
    ~EventLoop();

    // �����¼�ѭ��
    void loop();
    // �˳��¼�ѭ��
    void quit();

    // ��ȡPoller����ʱ���
    Timestamp pollReturnTime() const;

    // �ڵ�ǰloop��ִ��cb
    void runInLoop(Functor cb);
    // ��cb������У�����loop�����߳�ִ��cb
    void queueInLoop(Functor cb);

    // ����loop�����̣߳���������wakeUpfdд
    void wakeUp();

    // ����Poller����Ӧ����
    void updateChannel(Channel *channel);
    void removeChannel(Channel *channel);
    bool hasChannel(Channel *channel);

    // �ж�EventLoop�����Ƿ����Լ����߳���
    bool isInLoopThread() const;

private:
    // wakeUpChannel�ص�����������wakeUpfd��
    void handleRead();
    // ִ�лص�
    void doPendingFunctors();

    atomic_bool looping_; // ��ʶ�����¼�ѭ��
    atomic_bool quit_;    // ��ʶ�˳��¼�ѭ��

    const pid_t threadId_; // ��¼��ǰloop���ڵ��̵߳�ID

    Timestamp pollReturnTime_;  // ��¼Poller���ط����¼���Channels��ʱ���
    unique_ptr<Poller> poller_; // Poller���

    int wakeUpfd_; // ����ͳһ�¼�Դ������loop�߳�Ҳ���ü����׽��ֶ�д�¼�����
    unique_ptr<Channel> wakeUpChannel_;

    using ChannelList = vector<Channel *>;
    ChannelList activeChannels_; // ��Ծͨ���б�

    atomic_bool callingPendingFunctors_; // ��ʶ��ǰloop�Ƿ�����Ҫִ�еĻص�����
    vector<Functor> PendingFunctors_;    // �洢��ǰloop��Ҫִ�е����еĻص�����
    mutex mutex_;                        // ���ڱ�������vector�̰߳�ȫ�Ļ�����
};

#endif