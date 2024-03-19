#ifndef __TCPCONNECTION_H__
#define __TCPCONNECTION_H__

#include "Buffer.h"
#include "Callbacks.h"
#include "Channel.h"
#include "EventLoop.h"
#include "InetAddress.h"
#include "Log.h"
#include "Socket.h"
#include "Timestamp.h"
#include "noncopyable.h"

#include <atomic>
#include <errno.h>
#include <functional>
#include <memory>
#include <netinet/tcp.h>
#include <string>
#include <strings.h>
#include <sys/socket.h>
#include <sys/types.h>
using std::atomic_int;
using std::bind;
using std::enable_shared_from_this;
using namespace std::placeholders;
using std::string;
using std::unique_ptr;

class TcpConnection : noncopyable,
                      public enable_shared_from_this<TcpConnection> {
public:
    TcpConnection(EventLoop *loop, const string name, int socketfd,
                  const InetAddress &localAddr, const InetAddress &peerAddr);
    ~TcpConnection();

    EventLoop *getLoop() const;
    const string &name() const;
    const InetAddress &localAddress() const;
    const InetAddress &peerAddress() const;

    bool connected() const;

    // ��������
    void send(const string &buf);

    // �ر�����
    void shutdown();

    void setConnectionCallback(const ConnectionCallback &cb);

    void setMessageCallback(const MessageCallback &cb);

    void setWriteCompleteCallback(const WriteCompleteCallback &cb);

    void setHighWaterMarkCallback(const HighWaterMarkCallback &cb, size_t highWaterMark);

    void setCloseCallback(const CloseCallback &cb);

    // ���ӽ���
    void connectEstablished();
    // ��������
    void connectDestroyed();

private:
    enum STATE { kDisconnected,
                 kConnected,
                 kDisconnecting,
                 kConnecting };
    // ����״̬
    void setState(STATE state);

    // �ص�����
    void handleRead(Timestamp receiveTime);
    void handlelWrite();
    void handleClose();
    void handleError();

    void sendInLoop(const void *message, size_t len);

    void shutdownInLoop();

    EventLoop *loop_;
    const string name_;
    atomic_int state_;
    bool reading_;

    unique_ptr<Socket> socket_;
    unique_ptr<Channel> channel_;

    const InetAddress localAddress_;
    const InetAddress peerAddress_;

    ConnectionCallback connectionCallback_;       // �������ӻص�
    MessageCallback messageCallback_;             // �ж�д��Ϣ�ص�
    WriteCompleteCallback writeCompleteCallback_; // ��Ϣ������ɻص�
    HighWaterMarkCallback highWaterMarkCallback_; // ��ˮλ�ص�
    CloseCallback closeCallback_;                 // �ر����ӻص�

    size_t highWaterMark_;

    Buffer inputBuffer_;  // �������ݻ�����
    Buffer outputBuffer_; // �������ݻ�����
};

#endif