#include "Time.hpp"

const map<string, Time> TimeInfo::strMap = {
    {"Day", Day}, {"Night", Night}
};

Time TimeInfo::parse(const string& s) {
    auto it = strMap.find(s);
    return it != strMap.end() ? it->second : TIME_ERROR;
}
