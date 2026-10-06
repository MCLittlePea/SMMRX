#include "GameStyle.hpp"

const map<string, GameStyle> GameStyleInfo::strMap = {
    {"SMB1", SMB1}, {"SMB3", SMB3}, {"SMW", SMW}, {"NSMBU", NSMBU}, {"SM3DW", SM3DW}
};

GameStyle GameStyleInfo::parse(const string& s) {
    auto it = strMap.find(s);
    return it != strMap.end() ? it->second : STYLE_ERROR;
}
