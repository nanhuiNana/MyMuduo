#ifndef __CURRENTTHREAD_H__
#define __CURRENTTHREAD_H__

#include <sys/syscall.h>
#include <unistd.h>

/*获取线程ID封装*/
namespace CurrentThread {
extern __thread int t_cachedTid;

// 缓存当前线程的线程ID
void cacheTid();

// 获取当前线程的线程ID
inline int tid() {
    //__builtin_expect告诉编译器不为0的概率很小，有助于编译器对代码进行优化
    if (__builtin_expect(t_cachedTid == 0, 0)) {
        cacheTid();
    }
    return t_cachedTid;
}

}; // namespace CurrentThread

#endif