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

    // 发送数据
    void send(const string &buf);

    // 关闭连接
    void shutdown();

    void setConnectionCallback(const ConnectionCallback &cb);

    void setMessageCallback(const MessageCallback &cb);

    void setWriteCompleteCallback(const WriteCompleteCallback &cb);

    void setHighWaterMarkCallback(const HighWaterMarkCallback &cb, size_t highWaterMark);

    void setCloseCallback(const CloseCallback &cb);

    // 连接建立
    void connectEstablished();
    // 连接销毁
    void connectDestroyed();

private:
    enum STATE { kDisconnected,
                 kConnected,
                 kDisconnecting,
                 kConnecting };
    // 设置状态
    void setState(STATE state);

    // 回调函数
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

    ConnectionCallback connectionCallback_;       // 有新连接回调
    MessageCallback messageCallback_;             // 有读写信息回调
    WriteCompleteCallback writeCompleteCallback_; // 信息发送完成回调
    HighWaterMarkCallback highWaterMarkCallback_; // 高水位回调
    CloseCallback closeCallback_;                 // 关闭连接回调

    size_t highWaterMark_;

    Buffer inputBuffer_;  // 接收数据缓冲区
    Buffer outputBuffer_; // 发送数据缓冲区
};

#endif