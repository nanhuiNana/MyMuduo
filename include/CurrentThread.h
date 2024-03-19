#ifndef __CURRENTTHREAD_H__
#define __CURRENTTHREAD_H__

#include <sys/syscall.h>
#include <unistd.h>

/*��ȡ�߳�ID��װ*/
namespace CurrentThread {
extern __thread int t_cachedTid;

// ���浱ǰ�̵߳��߳�ID
void cacheTid();

// ��ȡ��ǰ�̵߳��߳�ID
inline int tid() {
    //__builtin_expect���߱�������Ϊ0�ĸ��ʺ�С�������ڱ������Դ�������Ż�
    if (__builtin_expect(t_cachedTid == 0, 0)) {
        cacheTid();
    }
    return t_cachedTid;
}

}; // namespace CurrentThread

#endif