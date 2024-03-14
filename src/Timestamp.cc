#include "../include/Timestamp.h"

Timestamp::Timestamp()
    : microSecondsSinceEpoch_(0) {
}

Timestamp::Timestamp(int64_t microSecondsSinceEpoch)
    : microSecondsSinceEpoch_(microSecondsSinceEpoch) {
}

Timestamp Timestamp::now() {
    return Timestamp(time(NULL));
}

string Timestamp::toString() const {
    char buf[128] = {0};
    tm *tmTime = localtime(&microSecondsSinceEpoch_);
    snprintf(buf, 128, "%04d/%02d/%02d %02d:%02d:%02d",
             tmTime->tm_year + 1900,
             tmTime->tm_mon + 1,
             tmTime->tm_mday,
             tmTime->tm_hour,
             tmTime->tm_min,
             tmTime->tm_sec);
    return buf;
}

/*²âÊÔ³ÌĞò*/
/*
int main() {
    std::cout << Timestamp::now().toString() << std::endl;
    return 0;
}
*/