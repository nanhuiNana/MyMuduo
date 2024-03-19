#include "../include/CurrentThread.h"

namespace CurrentThread {
__thread int t_cachedTid = 0; //__thread表示该变量是线程局部的，每一个线程都要一份独立的拷贝

void cacheTid() {
    // 如果没有缓存
    if (t_cachedTid == 0) {
        // 调用系统函数获取线程ID
        t_cachedTid = static_cast<pid_t>(::syscall(SYS_gettid));
    }
}

}; // namespace CurrentThread