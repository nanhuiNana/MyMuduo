#ifndef __INETADDRESS_H__
#define __INETADDRESS_H__

#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <string>
#include <strings.h>
using std::string;

/*地址结构类*/
class InetAddress {
public:
    // 通过IP和端口号进行构造
    explicit InetAddress(uint16_t port = 0, string ip = "127.0.0.1");
    // 通过地址结构进行构造
    explicit InetAddress(const sockaddr_in &addr);

    // 获取IP
    string toIp() const;
    // 获取端口号
    uint16_t toPort() const;
    // 获取IP和端口号
    string toIpPort() const;

    // 获取地址结构
    const sockaddr_in *getSocketAddr() const;
    // 设置地址结构
    void setSocketAddr(const sockaddr_in &addr);

private:
    sockaddr_in addr_; // 地址结构
};

#endif