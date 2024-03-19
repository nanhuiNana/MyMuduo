#ifndef __ACCEPTOR_H__
#define __ACCEPTOR_H__

#include "Channel.h"
#include "Socket.h"
#include "noncopyable.h"

#include <errno.h>
#include <functional>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
using std::bind;
using std::function;

class InetAddress;
class EventLoop;

class Acceptor : noncopyable {
public:
    using NewConnectionCallback = function<void(int socketfd, const InetAddress &)>;

    Acceptor(EventLoop *loop, const InetAddress &listenAddr, bool reusePort);
    ~Acceptor();

    void setNewConnectionCallback(const NewConnectionCallback &cb);

    bool listenning() const;
    void listen();

private:
    void handleRead();

    EventLoop *loop_;
    Socket acceptSocket_;
    Channel acceptChannel_;
    NewConnectionCallback newConnectionCallback_;
    bool listenning_;
};

#endif