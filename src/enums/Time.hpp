#ifndef ENUMS_TIME_HPP
#define ENUMS_TIME_HPP

#include <map>
#include <string>

using namespace std;

enum Time {
    Day,
    Night,
    TIME_ERROR
};

class TimeInfo {
public:
    static const map<string, Time> strMap;
    static Time parse(const string& s);
};

#endif
