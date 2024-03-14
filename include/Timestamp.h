#ifndef __TIMESTAMP_H__
#define __TIMESTAMP_H__

#include <iostream>
#include <string>
#include <time.h>
using std::string;
class Timestamp {
public:
    // �޲ι���
    Timestamp();
    // �вι���
    explicit Timestamp(int64_t microSecondsSinceEpoch);
    // ��ȡ��ǰʱ��
    static Timestamp now();
    // ��ȡ��ʽ��ʱ���ַ���
    string toString() const;

private:
    int64_t microSecondsSinceEpoch_; // ʹ��64λint�洢ʱ��
};

#endif