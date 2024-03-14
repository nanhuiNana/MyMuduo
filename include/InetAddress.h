#ifndef __INETADDRESS_H__
#define __INETADDRESS_H__

#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <string>
#include <strings.h>
using std::string;

/*��ַ�ṹ��*/
class InetAddress {
public:
    // ͨ��IP�Ͷ˿ںŽ��й���
    explicit InetAddress(uint16_t port = 0, string ip = "127.0.0.1");
    // ͨ����ַ�ṹ���й���
    explicit InetAddress(const sockaddr_in &addr);

    // ��ȡIP
    string toIp() const;
    // ��ȡ�˿ں�
    uint16_t toPort() const;
    // ��ȡIP�Ͷ˿ں�
    string toIpPort() const;

    // ��ȡ��ַ�ṹ
    const sockaddr_in *getSocketAddr() const;
    // ���õ�ַ�ṹ
    void setSocketAddr(const sockaddr_in &addr);

private:
    sockaddr_in addr_; // ��ַ�ṹ
};

#endif