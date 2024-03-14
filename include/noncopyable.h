#ifndef __NONCOPYABLE_H__
#define __NONCOPYABLE_H__

/**
 * noncopyable的派生类可以正常的构造和析构，但是无法进行拷贝构造和拷贝赋值
 */
class noncopyable {
public:
    noncopyable(noncopyable &) = delete;
    noncopyable &operator=(const noncopyable &) = delete;

protected:
    noncopyable() = default;
    ~noncopyable() = default;
};

#endif