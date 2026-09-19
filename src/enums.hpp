#ifndef ENUMS_HPP
#define ENUMS_HPP

#include <map>
#include <string>

using namespace std;

enum GameState {
    STATE_START,
    STATE_ANIMATION,
    STATE_GAME,
    STATE_DEAD,
    STATE_ERROR
};

enum Language {
    English,
    Chinese,
    LanguageCount,
    LanguageError
};

enum Character {
    MARIO,
    LUIGI,
    TOAD,
    TOADETTE,
    CHAR_ERROR
};

enum GameStyle {
    SMB1,
    SMB3,
    SMW,
    NSMBU,
    SM3DW,
    STYLE_ERROR
};

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

enum Time {
    Day,
    Night,
    TIME_ERROR
};

enum BGMType {
    Edit,
    PlayNormal,
    PlayMoon,
    PlayHurry,
    PlayMoonHurry,
    Hurry,
    BGM_TYPE_COUNT,
    BGM_TYPE_ERROR
};

enum GuiElementType {
    NUMBER_FONT,
    GUI_ELEMENT_TYPE_COUNT,
    GUI_ELEMENT_ERROR
};

enum Ability {
    Small,
    Super,
    Fire,
    Big,
    SMB2,
    Link,
    SuperBall,
    Racoon,
    Frog,
    Cape,
    Balloon,
    Propeller,
    FlyingSquirrel,
    Cat,
    Boomerang,
    Builder,
    ABILITY_ERROR
};

inline const map<string, GameStyle> gameStyleMap = {
    {"SMB1", SMB1}, {"SMB3", SMB3}, {"SMW", SMW}, {"NSMBU", NSMBU}, {"SM3DW", SM3DW}
};

inline const map<string, CourseTheme> courseThemeMap = {
    {"Ground", Ground}, {"Underground", Underground}, {"Underwater", Underwater},
    {"Desert", Desert}, {"Snow", Snow}, {"Sky", Sky}, {"Forest", Forest},
    {"GhostHouse", GhostHouse}, {"Airship", Airship}, {"Castle", Castle}
};

inline const map<string, Time> timeMap = {
    {"Day", Day}, {"Night", Night}
};

inline const map<string, Character> characterMap = {
    {"MARIO", MARIO}, {"LUIGI", LUIGI}, {"TOAD", TOAD}, {"TOADETTE", TOADETTE}
};

inline const map<string, Ability> abilityMap = {
    {"Small", Small}, {"Nothing", Small}, {"Super", Super}, {"Fire", Fire}, {"Big", Big},
    {"SMB2", SMB2}, {"Link", Link}, {"SuperBall", SuperBall}, {"Racoon", Racoon},
    {"Frog", Frog}, {"Cape", Cape}, {"Balloon", Balloon}, {"Propeller", Propeller},
    {"FlyingSquirrel", FlyingSquirrel}, {"Cat", Cat}, {"Boomerang", Boomerang},
    {"Builder", Builder}
};

inline const map<string, GuiElementType> guiElementTypeMap = {
    {"NUMBER_FONT", NUMBER_FONT}
};

inline GameStyle parseGameStyle(const string& s) {
    auto it = gameStyleMap.find(s);
    return it != gameStyleMap.end() ? it->second : STYLE_ERROR;
}

inline CourseTheme parseCourseTheme(const string& s) {
    auto it = courseThemeMap.find(s);
    return it != courseThemeMap.end() ? it->second : THEME_ERROR;
}

inline Time parseTime(const string& s) {
    auto it = timeMap.find(s);
    return it != timeMap.end() ? it->second : TIME_ERROR;
}

inline Character parseCharacter(const string& s) {
    auto it = characterMap.find(s);
    return it != characterMap.end() ? it->second : CHAR_ERROR;
}

inline Ability parseAbility(const string& s) {
    auto it = abilityMap.find(s);
    return it != abilityMap.end() ? it->second : ABILITY_ERROR;
}

inline GuiElementType parseGuiElementType(const string& s) {
    auto it = guiElementTypeMap.find(s);
    return it != guiElementTypeMap.end() ? it->second : GUI_ELEMENT_ERROR;
}

#endif
