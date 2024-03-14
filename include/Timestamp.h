#ifndef __TIMESTAMP_H__
#define __TIMESTAMP_H__

#include <iostream>
#include <string>
#include <time.h>
using std::string;
class Timestamp {
public:
    // 无参构造
    Timestamp();
    // 有参构造
    explicit Timestamp(int64_t microSecondsSinceEpoch);
    // 获取当前时间
    static Timestamp now();
    // 获取格式化时间字符串
    string toString() const;

private:
    int64_t microSecondsSinceEpoch_; // 使用64位int存储时间
};

#endif