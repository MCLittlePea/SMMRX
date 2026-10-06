#include "CourseTheme.hpp"

const map<string, CourseTheme> CourseThemeInfo::strMap = {
    {"Ground", Ground}, {"Underground", Underground}, {"Underwater", Underwater},
    {"Desert", Desert}, {"Snow", Snow}, {"Sky", Sky}, {"Forest", Forest},
    {"GhostHouse", GhostHouse}, {"Airship", Airship}, {"Castle", Castle}
};

CourseTheme CourseThemeInfo::parse(const string& s) {
    auto it = strMap.find(s);
    return it != strMap.end() ? it->second : THEME_ERROR;
}
