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

/*��װ�̲߳���*/
class Thread : noncopyable {
public:
    using ThreadFunc = function<void()>;

    explicit Thread(ThreadFunc, const string &name = string());
    ~Thread();

    // �����߳�
    void start();
    // �ȴ��߳�
    void join();

    // ��ȡ�߳�����״̬
    bool started() const;
    // ��ȡ�߳�id
    pid_t tid() const;
    // ��ȡ�߳�����
    const string &name() const;

    // ��ȡ�߳�����
    static int numCreated();

private:
    // �����̵߳�Ĭ������
    void setDefaultName();

    bool started_;                 // ��������
    bool join_;                    // �ȴ�����
    shared_ptr<thread> thread_;    // ʹ��������������߳�
    pid_t tid_;                    // ��¼�߳�id
    ThreadFunc func_;              // ��¼�̺߳���
    string name_;                  // �߳�����
    static atomic_int numCreated_; // �߳�������
};

#endif