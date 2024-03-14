#include "../include/InetAddress.h"

InetAddress::InetAddress(uint16_t port, string ip) {
    // ��ַ�ṹ����
    bzero(&addr_, sizeof addr_);
    // ����ַ�ṹ��ֵ������ת���磩
    addr_.sin_family = AF_INET;
    addr_.sin_port = htons(port);
    addr_.sin_addr.s_addr = inet_addr(ip.c_str());
}

InetAddress::InetAddress(const sockaddr_in &addr) : addr_(addr) {
}

string InetAddress::toIp() const {
    char buf[64] = {0};
    // ����ת����
    ::inet_ntop(AF_INET, &addr_.sin_addr, buf, sizeof buf);
    return buf;
}

uint16_t InetAddress::toPort() const {
    return ntohs(addr_.sin_port);
}

string InetAddress::toIpPort() const {
    char buf[64] = {0};
    ::inet_ntop(AF_INET, &addr_.sin_addr, buf, sizeof buf);
    size_t len = strlen(buf);
    uint16_t port = htons(addr_.sin_port);
    sprintf(buf + len, ":%u", port);
    return buf;
}

const sockaddr_in *InetAddress::getSocketAddr() const {
    return &addr_;
}

void InetAddress::setSocketAddr(const sockaddr_in &addr) {
    addr_ = addr;
}