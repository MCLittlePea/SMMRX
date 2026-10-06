#ifndef ENUMS_COURSE_THEME_HPP
#define ENUMS_COURSE_THEME_HPP

#include <map>
#include <string>

using namespace std;

enum CourseTheme {
    Ground,
    Underground,
    Underwater,
    Desert,
    Snow,
    Sky,
    Forest,
    GhostHouse,
    Airship,
    Castle,
    THEME_ERROR
};

class CourseThemeInfo {
public:
    static const map<string, CourseTheme> strMap;
    static CourseTheme parse(const string& s);
};

#endif
