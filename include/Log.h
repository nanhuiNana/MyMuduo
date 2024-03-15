#ifndef __LOG_H__
#define __LOG_H__

#include "Timestamp.h"
#include "noncopyable.h"

#include <iostream>
#include <string>
using std::cout;
using std::endl;
using std::string;

#define LOG_INFO(logMsgFormat, ...)                       \
    do {                                                  \
        Log &log = Log::getInstance();                    \
        log.setLogLevel(INFO);                            \
        char buf[1024] = {0};                             \
        snprintf(buf, 1024, logMsgFormat, ##__VA_ARGS__); \
        log.writeLog(buf);                                \
    } while (0)

#define LOG_ERROR(logMsgFormat, ...)                      \
    do {                                                  \
        Log &log = Log::getInstance();                    \
        log.setLogLevel(ERROR);                           \
        char buf[1024] = {0};                             \
        snprintf(buf, 1024, logMsgFormat, ##__VA_ARGS__); \
        log.writeLog(buf);                                \
    } while (0)

#define LOG_FATAL(logMsgFormat, ...)                      \
    do {                                                  \
        Log &log = Log::getInstance();                    \
        log.setLogLevel(FATAL);                           \
        char buf[1024] = {0};                             \
        snprintf(buf, 1024, logMsgFormat, ##__VA_ARGS__); \
        log.writeLog(buf);                                \
    } while (0)

#ifdef MUDUO_DEBUG
#define LOG_DEBUG(logMsgFormat, ...)                      \
    do {                                                  \
        Log &log = Log::getInstance();                    \
        log.setLogLevel(DEBUG);                           \
        char buf[1024] = {0};                             \
        snprintf(buf, 1024, logMsgFormat, ##__VA_ARGS__); \
        log.writeLog(buf);                                \
    } while (0)
#else
#define LOG_DEBUG(logMsgFormat, ...)
#endif

/*日志等级*/
enum LogLevel {
    INFO,  // 普通信息
    ERROR, // 错误信息
    FATAL, // 重大信息
    DEBUG  // 调试信息
};

/** 日志类
 * 继承noncopyable类，使其构造被保护起来，外部无法访问
 */
class Log : noncopyable {
public:
    // 获取日志类的唯一实例
    static Log &getInstance();
    // 设置日志等级
    void setLogLevel(int level);
    // 写日志
    void writeLog(string msg);

private:
    int logLevel_; // 日志等级
};

#endif