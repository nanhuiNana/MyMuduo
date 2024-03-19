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

/*网络库底层缓冲区封装*/
class Buffer {
public:
    static const size_t kCheapPrepend = 8;
    static const size_t kInitialSize = 1024;

    explicit Buffer(size_t initialSize = kInitialSize);

    // 可读字节数
    size_t readableBytes() const;
    // 可写字节数
    size_t writableBytes() const;
    // 可预置字节数
    size_t prependableBytes() const;

    // 缓冲区可读数据起始地址
    const char *peek() const;

    // 复位读指针
    void retrieve(size_t len);
    // 复位读写指针
    void retrieveAll();

    // 将全部可读数据转换成string
    string retrieveAllAsString();

    // 将指定长度可读数据转换成string
    string retrieveAsString(size_t len);

    // 确保缓冲区可写区域长度足够
    void ensureWritableBytes(size_t len);

    // 将data上的数据读取len字节长度到可写缓冲区
    void append(const char *data, size_t len);

    // 缓冲区可写区域起始地址
    char *beginWrite();
    // 缓冲区可写区域起始地址
    const char *beginWrite() const;

    // 从fd读取数据
    size_t readFd(int fd, int *saveErrno);

    // 往fd发送数据
    size_t writeFd(int fd, int *saveErrno);

private:
    // 获取vector首地址裸指针
    char *begin();
    // 获取vector首地址裸指针
    const char *begin() const;

    // 空间扩容
    void makeSpace(size_t len);

    vector<char> buffer_;
    size_t readerIndex_;
    size_t writerIndex_;
};

#endif