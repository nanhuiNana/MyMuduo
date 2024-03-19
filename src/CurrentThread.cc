#include "../include/CurrentThread.h"

namespace CurrentThread {
__thread int t_cachedTid = 0; //__thread��ʾ�ñ������ֲ߳̾��ģ�ÿһ���̶߳�Ҫһ�ݶ����Ŀ���

void cacheTid() {
    // ���û�л���
    if (t_cachedTid == 0) {
        // ����ϵͳ������ȡ�߳�ID
        t_cachedTid = static_cast<pid_t>(::syscall(SYS_gettid));
    }
}

}; // namespace CurrentThread