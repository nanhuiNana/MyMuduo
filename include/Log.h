#ifndef __LOG_H__
#define __LOG_H__

#include "Timestamp.h"
#include "noncopyable.h"

#include <iostream>
#include <string>
using std::cout;
using std::endl;
using std::string;

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