#ifndef __TCPSERVER_H__
#define __TCPSERVER_H__

#include "Acceptor.h"
#include "Buffer.h"
#include "Callbacks.h"
#include "EventLoop.h"
#include "EventLoopThreadPool.h"
#include "InetAddress.h"
#include "Log.h"
#include "TcpConnection.h"
#include "noncopyable.h"

#include <atomic>
#include <functional>
#include <memory>
#include <string>
#include <strings.h>
#include <unordered_map>
using std::atomic_int;
using std::function;
using std::shared_ptr;
using std::string;
using std::unique_ptr;
using std::unordered_map;

class TcpServer : noncopyable {
public:
    using ThreadInitCallback = std::function<void(EventLoop *)>;

    enum OPTION {
        kNoReusePort,
        kReusePort
    };

    TcpServer(EventLoop *loop,
              const InetAddress &listenAddr,
              const string &name,
              OPTION option = kNoReusePort);
    ~TcpServer();

    void setThreadInitCallback(const ThreadInitCallback &cb);
    void setConnectionCallback(const ConnectionCallback &cb);
    void setMessageCallback(const MessageCallback &cb);
    void setWriteCompleteCallback(const WriteCompleteCallback &cb);

    // 设置subloop个数
    void setThreadNum(int numThreads);

    // 开启服务器监听
    void start();

private:
    void newConnection(int socketfd, const InetAddress &peerAddr);
    void removeConnection(const TcpConnectionPtr &conn);
    void removeConnectionInLoop(const TcpConnectionPtr &conn);

    using ConnectionMap = unordered_map<string, TcpConnectionPtr>;

    EventLoop *loop_;

    const string ipPort_;
    const string name_;

    unique_ptr<Acceptor> acceptor_;

    shared_ptr<EventLoopThreadPool> threadPool_;

    ConnectionCallback connectionCallback_;
    MessageCallback messageCallback_;
    WriteCompleteCallback writeCompleteCallback_;

    ThreadInitCallback threadInitCallback_;

    atomic_int started_;

    int nextConnId_;

    ConnectionMap connections_;
};

#endif