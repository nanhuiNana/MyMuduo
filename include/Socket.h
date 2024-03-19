#ifndef __SOCKET_H__
#define __SOCKET_H__

#include "InetAddress.h"
#include "Log.h"
#include "noncopyable.h"

#include <netinet/tcp.h>
#include <strings.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

class InetAddress;

/*封装socket连接相关操作*/
class Socket : noncopyable {
public:
    explicit Socket(int socketfd);
    ~Socket();

    int fd() const;
    void bindAddress(const InetAddress &localAddr);
    void listen();
    int accept(InetAddress &peerAddr);

    void shutdownWrite();

    // 设置连接属性
    void setTcpNoDelay(bool on);
    void setReuseAddr(bool on);
    void setReusePort(bool on);
    void setKeepAlive(bool on);

private:
    const int socketfd_;
};

#endif