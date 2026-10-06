#ifndef LANGUAGE_MANAGER_HPP
#define LANGUAGE_MANAGER_HPP

#include <raylib.h>
#include <map>
#include <string>
#include "../enums.hpp"

using namespace std;

class LanguageManager {
public:
    static string currentLanguage;
    static map<string, map<string, string>> langData;
    static Font guiFont;

    static void loadLanguages();
    static void initFont();
    static const string& langText(const string& id);
    static const char* langTextC(const string& id);
    static void drawText(const char* text, float x, float y, float size, Color color);
    static int measureText(const char* text, float size);
};

#endif
