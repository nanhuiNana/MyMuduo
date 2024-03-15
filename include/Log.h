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

/*��־�ȼ�*/
enum LogLevel {
    INFO,  // ��ͨ��Ϣ
    ERROR, // ������Ϣ
    FATAL, // �ش���Ϣ
    DEBUG  // ������Ϣ
};

/** ��־��
 * �̳�noncopyable�࣬ʹ�乹�챻�����������ⲿ�޷�����
 */
class Log : noncopyable {
public:
    // ��ȡ��־���Ψһʵ��
    static Log &getInstance();
    // ������־�ȼ�
    void setLogLevel(int level);
    // д��־
    void writeLog(string msg);

private:
    int logLevel_; // ��־�ȼ�
};

#endif