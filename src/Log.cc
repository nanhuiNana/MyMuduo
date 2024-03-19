#include "../include/Log.h"

Log &Log::getInstance() {
    static Log log;
    return log;
}

void Log::setLogLevel(int level) {
    logLevel_ = level;
}

void Log::writeLog(string msg) {
    switch (logLevel_) {
    case INFO:
        cout << "[INFO]";
        break;
    case ERROR:
        cout << "[ERROR]";
        break;
    case FATAL:
        cout << "[FATAL]";
        break;
    case DEBUG:
        cout << "[DEBUG]";
        break;
    default:
        break;
    }
    cout << Timestamp::now().toString() << " : " << msg << endl;
}

/*²âÊÔ³ÌÐò*/
// int main() {
//     LOG_INFO("%s", "hello");
//     return 0;
// }