#ifndef ENUMS_GAME_STYLE_HPP
#define ENUMS_GAME_STYLE_HPP

#include <map>
#include <string>

using namespace std;

enum GameStyle {
    SMB1,
    SMB3,
    SMW,
    NSMBU,
    SM3DW,
    STYLE_ERROR
};

class GameStyleInfo {
public:
    static const map<string, GameStyle> strMap;
    static GameStyle parse(const string& s);
};

#endif
