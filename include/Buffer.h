#ifndef __BUFFER_H__
#define __BUFFER_H__

#include <algorithm>
#include <errno.h>
#include <string>
#include <sys/uio.h>
#include <unistd.h>
#include <vector>
using std::copy;
using std::string;
using std::vector;

/*�����ײ㻺������װ*/
class Buffer {
public:
    static const size_t kCheapPrepend = 8;
    static const size_t kInitialSize = 1024;

    explicit Buffer(size_t initialSize = kInitialSize);

    // �ɶ��ֽ���
    size_t readableBytes() const;
    // ��д�ֽ���
    size_t writableBytes() const;
    // ��Ԥ���ֽ���
    size_t prependableBytes() const;

    // �������ɶ�������ʼ��ַ
    const char *peek() const;

    // ��λ��ָ��
    void retrieve(size_t len);
    // ��λ��дָ��
    void retrieveAll();

    // ��ȫ���ɶ�����ת����string
    string retrieveAllAsString();

    // ��ָ�����ȿɶ�����ת����string
    string retrieveAsString(size_t len);

    // ȷ����������д���򳤶��㹻
    void ensureWritableBytes(size_t len);

    // ��data�ϵ����ݶ�ȡlen�ֽڳ��ȵ���д������
    void append(const char *data, size_t len);

    // ��������д������ʼ��ַ
    char *beginWrite();
    // ��������д������ʼ��ַ
    const char *beginWrite() const;

    // ��fd��ȡ����
    size_t readFd(int fd, int *saveErrno);

    // ��fd��������
    size_t writeFd(int fd, int *saveErrno);

private:
    // ��ȡvector�׵�ַ��ָ��
    char *begin();
    // ��ȡvector�׵�ַ��ָ��
    const char *begin() const;

    // �ռ�����
    void makeSpace(size_t len);

    vector<char> buffer_;
    size_t readerIndex_;
    size_t writerIndex_;
};

#endif