#include "../include/Buffer.h"

Buffer::Buffer(size_t initialSize)
    : buffer_(kCheapPrepend + kInitialSize),
      readerIndex_(kCheapPrepend),
      writerIndex_(kCheapPrepend) {
}

size_t Buffer::readableBytes() const {
    return writerIndex_ - readerIndex_;
}

size_t Buffer::writableBytes() const {
    return buffer_.size() - writerIndex_;
}

size_t Buffer::prependableBytes() const {
    return readerIndex_;
}

const char *Buffer::peek() const {
    return begin() + readerIndex_;
}

void Buffer::retrieve(size_t len) {
    if (len < readableBytes()) {
        readerIndex_ += len;
    } else {
        retrieveAll();
    }
}

void Buffer::retrieveAll() {
    readerIndex_ = writerIndex_ = kCheapPrepend;
}

string Buffer::retrieveAllAsString() {
    return retrieveAsString(readableBytes());
}

string Buffer::retrieveAsString(size_t len) {
    string result(peek(), len);
    retrieve(len);
    return result;
}

void Buffer::ensureWritableBytes(size_t len) {
    if (writableBytes() < len) {
        makeSpace(len);
    }
}

void Buffer::append(const char *data, size_t len) {
    ensureWritableBytes(len);
    copy(data, data + len, beginWrite());
    writerIndex_ += len;
}

char *Buffer::beginWrite() {
    return begin() + writerIndex_;
}

const char *Buffer::beginWrite() const {
    return begin() + writerIndex_;
}

size_t Buffer::readFd(int fd, int *saveErrno) {
    char extrabuf[65536] = {0}; // 64K额外缓冲区

    struct iovec vec[2];

    const size_t writable = writableBytes();
    vec[0].iov_base = begin() + writerIndex_;
    vec[0].iov_len = writable;
    vec[1].iov_base = extrabuf;
    vec[1].iov_len = sizeof extrabuf;

    const int iovcnt = (writable < sizeof extrabuf) ? 2 : 1;
    const size_t n = ::readv(fd, vec, iovcnt);
    if (n < 0) {
        *saveErrno = errno;
    } else if (n <= writable) { // 可写缓冲区足够储存读出的数据
        writerIndex_ += n;
    } else {                            // 可写缓冲区不够储存读出的数据
        writerIndex_ = buffer_.size();  // 更新写指针
        append(extrabuf, n - writable); // 将额外缓冲区数据加入缓冲区
    }
    return n;
}

size_t Buffer::writeFd(int fd, int *saveErrno) {
    const size_t n = ::write(fd, peek(), readableBytes());
    if (n < 0) {
        *saveErrno = errno;
    }
    return n;
}

char *Buffer::begin() {
    return &*buffer_.begin();
}

const char *Buffer::begin() const {
    return &*buffer_.begin();
}

void Buffer::makeSpace(size_t len) {
    if (writableBytes() + prependableBytes() < len + kCheapPrepend) {
        buffer_.resize(writerIndex_ + len);
    } else {
        size_t readable = readableBytes();
        copy(begin() + readerIndex_, begin() + writerIndex_, begin() + kCheapPrepend);
        readerIndex_ = kCheapPrepend;
        writerIndex_ = readerIndex_ + readable;
    }
}