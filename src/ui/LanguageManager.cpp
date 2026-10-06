#include "LanguageManager.hpp"
#include <fstream>
#include <sstream>
#include <vector>
#include <set>

string LanguageManager::currentLanguage = "Chinese";
map<string, map<string, string>> LanguageManager::langData;
Font LanguageManager::guiFont = {};

static map<string, string> parseLangJSON(const string& content) {
    map<string, string> result;
    size_t pos = 0;
    while (pos < content.size()) {
        size_t keyStart = content.find('"', pos);
        if (keyStart == string::npos) break;
        size_t keyEnd = content.find('"', keyStart + 1);
        if (keyEnd == string::npos) break;
        string key = content.substr(keyStart + 1, keyEnd - keyStart - 1);
        size_t colon = content.find(':', keyEnd);
        if (colon == string::npos) break;
        size_t valStart = content.find('"', colon);
        if (valStart == string::npos) break;
        size_t valEnd = content.find('"', valStart + 1);
        if (valEnd == string::npos) break;
        string value = content.substr(valStart + 1, valEnd - valStart - 1);
        result[key] = value;
        pos = valEnd + 1;
    }
    return result;
}

static vector<int> utf8ToCodepoints(const string& str) {
    vector<int> codepoints;
    size_t i = 0;
    while (i < str.size()) {
        unsigned char c = static_cast<unsigned char>(str[i]);
        int codepoint = 0;
        int len = 0;
        if (c < 0x80) { codepoint = c; len = 1; }
        else if ((c & 0xE0) == 0xC0) { codepoint = c & 0x1F; len = 2; }
        else if ((c & 0xF0) == 0xE0) { codepoint = c & 0x0F; len = 3; }
        else if ((c & 0xF8) == 0xF0) { codepoint = c & 0x07; len = 4; }
        for (int j = 1; j < len && i + j < str.size(); j++) {
            codepoint = (codepoint << 6) | (static_cast<unsigned char>(str[i+j]) & 0x3F);
        }
        codepoints.push_back(codepoint);
        i += len;
    }
    return codepoints;
}

static void loadLanguage(const string& lang, const string& path) {
    ifstream file(path);
    if (!file.is_open()) return;
    stringstream ss;
    ss << file.rdbuf();
    LanguageManager::langData[lang] = parseLangJSON(ss.str());
}

void LanguageManager::loadLanguages() {
    loadLanguage("English", "assets/lang/en.json");
    loadLanguage("Chinese", "assets/lang/zh.json");
}

void LanguageManager::initFont() {
    set<int> codepointSet;
    for (int i = 32; i < 127; i++) codepointSet.insert(i);
    for (auto& [lang, texts] : langData) {
        for (auto& [key, value] : texts) {
            for (int cp : utf8ToCodepoints(value)) {
                codepointSet.insert(cp);
            }
        }
    }
    vector<int> chars(codepointSet.begin(), codepointSet.end());
    guiFont = LoadFontEx("assets/fonts/Zpix.ttf", 48, chars.data(), static_cast<int>(chars.size()));
    if (guiFont.texture.id == 0) TraceLog(LOG_WARNING, "Failed to load font");
    SetTextureFilter(guiFont.texture, TEXTURE_FILTER_POINT);
}

const string& LanguageManager::langText(const string& id) {
    static string missing;
    auto it = langData.find(currentLanguage);
    if (it != langData.end()) {
        auto it2 = it->second.find(id);
        if (it2 != it->second.end()) return it2->second;
    }
    missing = id;
    return missing;
}

const char* LanguageManager::langTextC(const string& id) {
    return langText(id).c_str();
}

void LanguageManager::drawText(const char* text, float x, float y, float size, Color color) {
    DrawTextEx(guiFont, text, {x, y}, size, 1, color);
}

int LanguageManager::measureText(const char* text, float size) {
    return static_cast<int>(MeasureTextEx(guiFont, text, size, 1).x);
}
