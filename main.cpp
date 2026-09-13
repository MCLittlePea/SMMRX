#include <raylib.h>
#include <cmath>
#include <map>
#include <set>
#include <string>
#include <algorithm>
#include <vector>
#include <fstream>
#include <sstream>
#include "player_settings.hpp"
#include "render_settings.hpp"
#include "window_settings.hpp"
#include "course_settings.hpp"
#include "animation_settings.hpp"

#define ENABLE_CAMERA_BOUNDS 1
#define ENABLE_ANIMATION 1

using namespace std;

enum GameState {
    STATE_START,
    STATE_ANIMATION,
    STATE_GAME,
    STATE_DEAD
};

enum Language {
    English,
    Chinese,
    LanguageCount
};

enum Character {
    MARIO,
    LUIGI,
    TOAD,
    TOADETTE
};

enum GameStyle {
    SMB1,
    SMB3,
    SMW,
    NSMBU,
    SM3DW
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
    Castle
};

enum Time {
    Day,
    Night
};

enum BGMType {
    Edit,
    PlayNormal,
    PlayMoon,
    PlayHurry,
    PlayMoonHurry,
    Hurry,
    BGM_TYPE_COUNT
};

enum GuiElementType {
    NUMBER_FONT,
    GUI_ELEMENT_TYPE_COUNT
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
    Builder
};

struct BlockPos {
    int x;
    int y;

    bool operator<(const BlockPos& o) const {
        if (x != o.x) return x < o.x;
        return y < o.y;
    }
    bool operator==(const BlockPos& o) const {
        if (x != o.x) return false;
        if (y != o.y) return false;
        return true;
    }
};

struct PlacedBlock {
    BlockPos pos;
    string id;
};

struct WorldPos {
    float x;
    float y;
};

struct ScreenPos {
    float x;
    float y;
};

struct TextureRect {
    int x{};
    int y{};
    int width = 16;
    int height = 16;
};

struct CourseInfo {
    GameStyle style;
    CourseTheme theme;
    Time time;
    int maxTime = 300;

    bool operator<(const CourseInfo& o) const {
        if (style != o.style) return style < o.style;
        if (theme != o.theme) return theme < o.theme;
        return time < o.time;
    }

    bool operator==(const CourseInfo& o) const {
        if (style != o.style) return false;
        if (theme != o.theme) return false;
        return time == o.time;
    }
};

struct BlockTexture2DInfo {
    CourseInfo info;
    string id;
    float scale = 4.0f;

    bool operator<(const BlockTexture2DInfo& o) const {
        if (info != o.info) return info < o.info;
        if (id != o.id) return id < o.id;
        return scale < o.scale;
    }
};

struct PlayerTexture2DInfo {
    GameStyle style;
    Character player;
    Ability ability;
    float scale = 4.0f;

    bool operator<(const PlayerTexture2DInfo& o) const {
        if (style != o.style) return style < o.style;
        if (player != o.player) return player < o.player;
        if (ability != o.ability) return ability < o.ability;
        return scale < o.scale;
    }
};

struct BGMInfo {
    GameStyle style;
    CourseTheme theme;
    BGMType type;

    bool operator<(const BGMInfo& o) const {
        if (style != o.style) return style < o.style;
        if (theme != o.theme) return theme < o.theme;
        return type < o.type;
    }
};

struct GuiTextureInfo {
    GameStyle style;
    GuiElementType type;
    float scale = 4.0f;

    bool operator<(const GuiTextureInfo& o) const {
        if (style != o.style) return style < o.style;
        if (type != o.type) return type < o.type;
        return scale < o.scale;
    }
};

Character currentPlayer = MARIO;
Ability currentAbility = Small;
float currentTime;
float playerStateTimer = 0.0f;
map<string, int> playerFrames = {
    {"climb0", 0}, {"climb1", 1}, {"dead0", 2}, {"jump0", 3},
    {"stand0", 4}, {"stoop0", 5}, {"swim0", 6}, {"swim1", 7},
    {"swim2", 8}, {"swim3", 9}, {"swim4", 10}, {"swim5", 11},
    {"turn0", 12}, {"walk0", 13}, {"walk1", 14}, {"walk2", 15}
};
int playerAnimFrame = 0;
float playerAnimTimer = 0.0f;
bool playerFacingRight = true;
CourseInfo currentCourseInfo = {SMB1, Ground, Day, 70};

map<BlockTexture2DInfo, vector<Texture2D>> blockTextures;
map<PlayerTexture2DInfo, vector<Texture2D>> playerTextures;
struct BGMData {
    Music music;
    float loopStart;
    float loopLength;
    bool hasLoop;
};
map<BGMInfo, BGMData> bgms;
struct SoundEffect {
    Sound sound;
    float volume = 1.0f;
};

map<string, SoundEffect> soundEffects;
map<string, pair<float, float>> bgmLoopSamples;
float musicLogicalPos = 0.0f;
float currentBGMLoopStart = 0.0f;
float currentBGMLoopEnd = 0.0f;
bool currentBGMHasLoop = false;
map<GuiTextureInfo, vector<Texture2D>> guiTextures;
Music currentBGM = {};
bool hasCurrentBGM = false;
Music hurryUpSe = {};
bool playingHurryUp = false;
float hurryUpTimer = 0.0f;
bool hurryUpTriggered = false;
GameStyle hurryUpPendingStyle;
CourseTheme hurryUpPendingTheme;
BGMType hurryUpPendingType;
float bgmVolume = 1.0f;
float bgmFadeStartVolume = 0.0f;
float bgmFadeTargetVolume = 1.0f;
float bgmFadeTimer = 0.0f;
float bgmFadeDuration = 0.0f;
bool bgmFading = false;
bool gameOver = false;
bool isDead = false;
float deathWaitTimer = 0.0f;
bool deathJumped = false;
bool deathBounce = true;
#if ENABLE_ANIMATION
GameState state = STATE_START;
#else
GameState state = STATE_ANIMATION;
#endif
float playerXSpeed = 0.0f;
float playerYSpeed = 0.0f;
bool debugMode = false;
Language currentLanguage = Chinese;
Font guiFont;
map<Language, map<string, string>> langData;

struct CollisionBox {
    float x, y, width, height;
};
CollisionBox playerBox;
map<CourseInfo, vector<Texture2D>> backgrounds;
int bgAnimFrame = 0;
float bgAnimTimer = 0.0f;

vector<PlacedBlock> levelBlocks;

void loadSoundEffect(const string& id, const string& path, float volume = 1.0f) {
    soundEffects[id] = {LoadSound(path.c_str()), volume};
}

void playSoundEffect(const string& id) {
    auto it = soundEffects.find(id);
    if (it != soundEffects.end()) {
        SetSoundVolume(it->second.sound, it->second.volume);
        PlaySound(it->second.sound);
    }
}

map<string, string> parseLangJSON(const string& content) {
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

void loadLanguage(Language lang, const string& path) {
    ifstream file(path);
    if (!file.is_open()) return;
    stringstream ss;
    ss << file.rdbuf();
    langData[lang] = parseLangJSON(ss.str());
}

void loadLanguages() {
    loadLanguage(English, "assets/lang/en.json");
    loadLanguage(Chinese, "assets/lang/zh.json");
}

const string& langText(const string& id) {
    static string missing;
    auto it = langData.find(currentLanguage);
    if (it != langData.end()) {
        auto it2 = it->second.find(id);
        if (it2 != it->second.end()) return it2->second;
    }
    missing = id;
    return missing;
}

const char* langTextC(const string& id) {
    return langText(id).c_str();
}

vector<int> utf8ToCodepoints(const string& str) {
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

void initFont() {
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

void drawText(const char* text, float x, float y, float size, Color color) {
    DrawTextEx(guiFont, text, {x, y}, size, 1, color);
}

int measureText(const char* text, float size) {
    return static_cast<int>(MeasureTextEx(guiFont, text, size, 1).x);
}

void addBlock(BlockPos pos, const string& id) {
    levelBlocks.push_back({pos, id});
}

void initLevel() {
    for (int i = 1; i <= 10; i++) addBlock({i, 28}, "ground");
    for (int i = 1; i <= 10; i++) addBlock({i, 27}, "ground");
    for (int i = 12; i <= 200; i++) addBlock({i, 28}, "ground");
    for (int i = 12; i <= 200; i++) addBlock({i, 27}, "ground");
    addBlock({5,25}, "ground");
    addBlock({5,24}, "ground");
    addBlock({5,23}, "ground");
    addBlock({5,22}, "ground");
    addBlock({5,21}, "ground");
    addBlock({2,24},"hard_block");
    addBlock({3,24},"hard_block");
    addBlock({7,24},"hard_block");
    addBlock({8,24},"hard_block");
}


map<string, pair<float, float>> blockBoxSizes = {
    {"ground", {1.0f, 1.0f}},
    {"brick_block", {1.0f, 1.0f}},
    {"hard_block", {1.0f, 1.0f}},
};

bool boxOverlap(const CollisionBox& a, const CollisionBox& b) {
    return a.x < b.x + b.width && a.x + a.width > b.x &&
           a.y < b.y + b.height && a.y + a.height > b.y;
}

CollisionBox getBlockBox(const BlockPos& pos, const string& id) {
    auto it = blockBoxSizes.find(id);
    float w = (it != blockBoxSizes.end()) ? it->second.first : 1.0f;
    float h = (it != blockBoxSizes.end()) ? it->second.second : 1.0f;
    return {(float)(pos.x - 1) * BLOCK_PX, (float)(pos.y - 1) * BLOCK_PX, (float)BLOCK_PX * w, (float)BLOCK_PX * h};
}



struct HudManager {
    bool running;

    void init() { currentTime = currentCourseInfo.maxTime; }
    void start() { running = true; }
    void update(float dt) {
        if (running && currentTime > 0) {
            currentTime -= dt;
            if (currentTime < 0) currentTime = 0;
        }
    }
    void drawGlyph(GuiElementType type, int index, float x, float y) {
        auto it = guiTextures.find({currentCourseInfo.style, type});
        if (it == guiTextures.end() || index >= static_cast<int>(it->second.size())) return;
        Texture2D glyph = it->second[index];
        if (glyph.id == 0) return;
        DrawTextureEx(glyph, {x, y}, 0.0f, 1.0f, WHITE);
    }
    void draw() {
        int total = static_cast<int>(currentTime);
        if (total > 9999) total = 9999;
        string total_s = to_string(total);
        while (total_s.size() < 4) total_s = "0" + total_s;
        float x = SCREEN_WIDTH - 35 * 5.0f - 50.0f;
        float y = 32.0f;
        drawGlyph(NUMBER_FONT, 10, x, y);
        drawGlyph(NUMBER_FONT, total_s[0] - '0', x + 35 + 10, y);
        drawGlyph(NUMBER_FONT, total_s[1] - '0', x + 35 * 2 + 10, y);
        drawGlyph(NUMBER_FONT, total_s[2] - '0', x + 35 * 3 + 10, y);
        drawGlyph(NUMBER_FONT, total_s[3] - '0', x + 35 * 4 + 10, y);
    }
    void unload() {}
};

HudManager hud;

float cameraX = 0.0f;
float cameraY = -(COURSE_HEIGHT - SCREEN_HEIGHT);

WorldPos playerPos = {(3 - 1) * BLOCK_PX, (5 - 1) * BLOCK_PX};

void initBlock(const BlockTexture2DInfo& info, const Texture2D& tex) {
    blockTextures[info].push_back(tex);
}

void loadBlockFromSheet(const string& id, CourseInfo info,
                        const string& path, TextureRect rect) {
    Image sheet = LoadImage(path.c_str());
    Rectangle r = {static_cast<float>(rect.x), static_cast<float>(rect.y),
                   static_cast<float>(rect.width), static_cast<float>(rect.height)};
    Image sub = ImageFromImage(sheet, r);
    Texture2D tex = LoadTextureFromImage(sub);
    SetTextureFilter(tex, TEXTURE_FILTER_POINT);
    UnloadImage(sub);
    UnloadImage(sheet);
    initBlock({info, id}, tex);
}

void loadPlayerFrames(GameStyle style, Character player, Ability ability,
                      const string& path, int x, int y, int count, int frameW, int frameH) {
    Image sheet = LoadImage(path.c_str());
    for (int i = 0; i < count; i++) {
        Rectangle rect = {static_cast<float>(x + i * frameW), static_cast<float>(y),
                         static_cast<float>(frameW), static_cast<float>(frameH)};
        Image sub = ImageFromImage(sheet, rect);
        Texture2D tex = LoadTextureFromImage(sub);
        SetTextureFilter(tex, TEXTURE_FILTER_POINT);
        UnloadImage(sub);
        playerTextures[{style, player, ability}].push_back(tex);
    }
    UnloadImage(sheet);
}

string bgmInfoToLoopName(GameStyle style, CourseTheme theme, BGMType type) {
    string styleName = "SMB1";
    string themeName;
    switch (theme) {
        case Ground:      themeName = "Ground"; break;
        case Underground: themeName = "Underground"; break;
        case Underwater:  themeName = "Underwater"; break;
        case Desert:      themeName = "Desert"; break;
        case Snow:        themeName = "Snow"; break;
        case Sky:         themeName = "Sky"; break;
        case Forest:      themeName = "Forest"; break;
        case GhostHouse:  themeName = "GhostHouse"; break;
        case Airship:     themeName = "Airship"; break;
        case Castle:      themeName = "Castle"; break;
        default:          themeName = "Ground"; break;
    }
    string suffix;
    switch (type) {
        case PlayNormal:    suffix = ""; break;
        case PlayHurry:     suffix = "_Hurry"; break;
        case PlayMoon:      suffix = "_Moon"; break;
        case PlayMoonHurry: suffix = "_Moon_Hurry"; break;
        case Edit:          suffix = "_Edit"; break;
        default:            suffix = ""; break;
    }
    return styleName + "_" + themeName + suffix;
}

void loadBGMLoopsFromCSV(const string& path) {
    ifstream file(path);
    if (!file.is_open()) return;
    string line;
    getline(file, line);
    while (getline(file, line)) {
        if (line.empty()) continue;
        size_t p1 = line.find(',');
        size_t p2 = line.find(',', p1 + 1);
        if (p1 == string::npos || p2 == string::npos) continue;
        string name = line.substr(0, p1);
        float ls = stof(line.substr(p1 + 1, p2 - p1 - 1));
        float le = stof(line.substr(p2 + 1));
        bgmLoopSamples[name] = {ls, le};
    }
}

void loadBGM(GameStyle style, CourseTheme theme, BGMType type, const string& path) {
    Music m = LoadMusicStream(path.c_str());
    BGMData data;
    data.music = m;
    data.hasLoop = false;
    data.loopStart = 0.0f;
    data.loopLength = 0.0f;
    string loopName = bgmInfoToLoopName(style, theme, type);
    auto lit = bgmLoopSamples.find(loopName);
    if (lit != bgmLoopSamples.end()) {
        float ls = lit->second.first;
        float le = lit->second.second;
        data.loopStart = ls;
        data.loopLength = le - ls;
        data.hasLoop = true;
    }
    bgms[{style, theme, type}] = data;
}

void loadBGMFromTheme(GameStyle style, CourseTheme theme) {
    string themeName;
    switch (theme) {
        case Ground:      themeName = "Ground"; break;
        case Underground: themeName = "Underground"; break;
        case Underwater:  themeName = "Underwater"; break;
        case Desert:      themeName = "Desert"; break;
        case Snow:        themeName = "Snow"; break;
        case Sky:         themeName = "Sky"; break;
        case Forest:      themeName = "Forest"; break;
        case GhostHouse:  themeName = "GhostHouse"; break;
        case Airship:     themeName = "Airship"; break;
        case Castle:      themeName = "Castle"; break;
        default:          themeName = "Ground"; break;
    }
    string base = "assets/musics/SMB1/" + themeName;
    loadBGM(style, theme, PlayNormal,    base + ".mp3");
    loadBGM(style, theme, Edit,      base + "_Edit.mp3");
    loadBGM(style, theme, PlayMoon,      base + "_Moon.mp3");
    loadBGM(style, theme, PlayHurry,     base + "_Hurry.mp3");
    loadBGM(style, theme, PlayMoonHurry, base + "_Moon_Hurry.mp3");
}

void playBGM(GameStyle style, CourseTheme theme, BGMType type, bool alignPosition = false) {
    auto it = bgms.find({style, theme, type});
    if (it == bgms.end()) return;
    if (hasCurrentBGM) StopMusicStream(currentBGM);
    BGMData& data = it->second;
    currentBGM = data.music;
    PlayMusicStream(currentBGM);
    SetMusicVolume(currentBGM, bgmVolume);
    if (!alignPosition) {
        musicLogicalPos = 0.0f;
    }
    if (data.hasLoop) {
        currentBGMLoopStart = data.loopStart;
        currentBGMLoopEnd = data.loopStart + data.loopLength;
        currentBGMHasLoop = true;
        if (alignPosition) {
            float pos;
            if (musicLogicalPos < data.loopStart) {
                pos = musicLogicalPos;
            } else {
                pos = fmod(musicLogicalPos - data.loopStart, data.loopLength) + data.loopStart;
            }
            SeekMusicStream(currentBGM, pos);
        }
    } else {
        currentBGMHasLoop = false;
    }
    hasCurrentBGM = true;
}

void setMusicLogicalPos(float pos) {
    musicLogicalPos = pos;
    if (hasCurrentBGM && currentBGMHasLoop) {
        float actualPos;
        if (pos < currentBGMLoopStart) {
            actualPos = pos;
        } else {
            actualPos = fmod(pos - currentBGMLoopStart, currentBGMLoopEnd - currentBGMLoopStart) + currentBGMLoopStart;
        }
        SeekMusicStream(currentBGM, actualPos);
    }
}

void stopBGM() {
    if (hasCurrentBGM) StopMusicStream(currentBGM);
    hasCurrentBGM = false;
}

void setBGMVolume(float volume) {
    bgmVolume = volume;
    if (hasCurrentBGM) SetMusicVolume(currentBGM, bgmVolume);
}

void fadeBGM(float targetVolume, float duration) {
    bgmFadeStartVolume = bgmVolume;
    bgmFadeTargetVolume = targetVolume;
    bgmFadeTimer = 0.0f;
    bgmFadeDuration = duration;
    bgmFading = true;
}

void die(bool bounce = true) {
    if (isDead) return;
    isDead = true;
    state = STATE_DEAD;
    stopBGM();
    if (playingHurryUp) {
        playingHurryUp = false;
        StopMusicStream(hurryUpSe);
    }
    playerYSpeed = 0.0f;
    playerXSpeed = 0.0f;
    deathWaitTimer = 0.0f;
    deathJumped = false;
    deathBounce = bounce;
    playSoundEffect("player.death");
}

void updateBGMFade(float dt) {
    if (!bgmFading) return;
    bgmFadeTimer += dt;
    float t = bgmFadeDuration > 0 ? min(bgmFadeTimer / bgmFadeDuration, 1.0f) : 1.0f;
    bgmVolume = bgmFadeStartVolume + (bgmFadeTargetVolume - bgmFadeStartVolume) * t;
    if (hasCurrentBGM) SetMusicVolume(currentBGM, bgmVolume);
    if (t >= 1.0f) bgmFading = false;
}

void loadBackground(CourseInfo info, const string& path) {
    Texture2D tex = LoadTexture(path.c_str());
    SetTextureFilter(tex, TEXTURE_FILTER_POINT);
    backgrounds[info].push_back(tex);
}

GameStyle parseGameStyle(const string& s) {
    if (s == "SMB1") return SMB1;
    if (s == "SMB3") return SMB3;
    if (s == "SMW") return SMW;
    if (s == "NSMBU") return NSMBU;
    if (s == "SM3DW") return SM3DW;
    return SMB1;
}

CourseTheme parseCourseTheme(const string& s) {
    if (s == "Ground") return Ground;
    if (s == "Underground") return Underground;
    if (s == "Underwater") return Underwater;
    if (s == "Desert") return Desert;
    if (s == "Snow") return Snow;
    if (s == "Sky") return Sky;
    if (s == "Forest") return Forest;
    if (s == "GhostHouse") return GhostHouse;
    if (s == "Airship") return Airship;
    if (s == "Castle") return Castle;
    return Ground;
}

Time parseTime(const string& s) {
    if (s == "Night") return Night;
    return Day;
}

Character parseCharacter(const string& s) {
    if (s == "LUIGI") return LUIGI;
    if (s == "TOAD") return TOAD;
    if (s == "TOADETTE") return TOADETTE;
    return MARIO;
}

Ability parseAbility(const string& s) {
    if (s == "Super") return Super;
    if (s == "Fire") return Fire;
    if (s == "Big") return Big;
    if (s == "SMB2") return SMB2;
    if (s == "Link") return Link;
    if (s == "SuperBall") return SuperBall;
    if (s == "Racoon") return Racoon;
    if (s == "Frog") return Frog;
    if (s == "Cape") return Cape;
    if (s == "Balloon") return Balloon;
    if (s == "Propeller") return Propeller;
    if (s == "FlyingSquirrel") return FlyingSquirrel;
    if (s == "Cat") return Cat;
    if (s == "Boomerang") return Boomerang;
    if (s == "Builder") return Builder;
    return Small;
}

vector<string> splitCSV(const string& line) {
    vector<string> result;
    stringstream ss(line);
    string cell;
    while (getline(ss, cell, ',')) result.push_back(cell);
    return result;
}
GuiElementType parseGuiElementType(const string& s) {
    if (s == "NUMBER_FONT") return NUMBER_FONT;
    return NUMBER_FONT;
}

void loadGuiFromSheet(GuiElementType type, GameStyle style, const string& path, TextureRect rect) {
    Image sheet = LoadImage(path.c_str());
    Rectangle r = {static_cast<float>(rect.x), static_cast<float>(rect.y),
                   static_cast<float>(rect.width), static_cast<float>(rect.height)};
    Image sub = ImageFromImage(sheet, r);
    Texture2D tex = LoadTextureFromImage(sub);
    SetTextureFilter(tex, TEXTURE_FILTER_POINT);
    UnloadImage(sub);
    UnloadImage(sheet);
    guiTextures[{style, type}].push_back(tex);
}

void loadGuiFromCSV(const string& path) {
    ifstream file(path);
    string line;
    getline(file, line);
    while (getline(file, line)) {
        if (line.empty()) continue;
        auto c = splitCSV(line);
        if (c.size() < 7) continue;
        loadGuiFromSheet(parseGuiElementType(c[0]), parseGameStyle(c[1]),
                          c[2], {stoi(c[3]), stoi(c[4]), stoi(c[5]), stoi(c[6])});
    }
}


void loadBlocksFromCSV(const string& path) {
    ifstream file(path);
    string line;
    getline(file, line);
    while (getline(file, line)) {
        if (line.empty()) continue;
        auto c = splitCSV(line);
        if (c.size() < 9) continue;
        loadBlockFromSheet(c[0], {parseGameStyle(c[1]), parseCourseTheme(c[2]), parseTime(c[3])},
                            c[4], {stoi(c[5]), stoi(c[6]), stoi(c[7]), stoi(c[8])});
    }
}

void loadBackgroundsFromCSV(const string& path) {
    ifstream file(path);
    string line;
    getline(file, line);
    while (getline(file, line)) {
        if (line.empty()) continue;
        auto c = splitCSV(line);
        if (c.size() < 4) continue;
        loadBackground({parseGameStyle(c[0]), parseCourseTheme(c[1]), parseTime(c[2])}, c[3]);
    }
}

void loadPlayersFromCSV(const string& path) {
    ifstream file(path);
    string line;
    getline(file, line);
    while (getline(file, line)) {
        if (line.empty()) continue;
        auto c = splitCSV(line);
        if (c.size() < 9) continue;
        loadPlayerFrames(parseGameStyle(c[0]), parseCharacter(c[1]), parseAbility(c[2]),
                          c[3], stoi(c[4]), stoi(c[5]), stoi(c[6]), stoi(c[7]), stoi(c[8]));
    }
}

void loadSoundEffectsFromCSV(const string& path) {
    ifstream file(path);
    string line;
    getline(file, line);
    while (getline(file, line)) {
        if (line.empty()) continue;
        auto c = splitCSV(line);
        if (c.size() < 2) continue;
        float vol = (c.size() >= 3) ? stof(c[2]) : 1.0f;
        loadSoundEffect(c[0], c[1], vol);
    }
}

void updateBackgroundAnim(float dt) {
    auto it = backgrounds.find(currentCourseInfo);
    if (it == backgrounds.end() || it->second.size() <= 1) return;
    bgAnimTimer += dt;
    if (bgAnimTimer >= bgAnimFrameDuration) {
        bgAnimTimer = 0.0f;
        bgAnimFrame = (bgAnimFrame + 1) % static_cast<int>(it->second.size());
    }
}

void DrawBackground() {
    auto it = backgrounds.find(currentCourseInfo);
    if (it == backgrounds.end() || it->second.empty()) return;
    Texture2D tex = it->second[bgAnimFrame % static_cast<int>(it->second.size())];
    for (int i = 0; i * bgSize + bgOffsetX < COURSE_WIDTH; i++) {
        float px = floorf(i * bgSize + bgOffsetX + cameraX);
        float py = floorf(bgY + cameraY);
        DrawTextureEx(tex, (Vector2){px, py}, 0.0f, static_cast<float>(SCALE), WHITE);
    }
}

void loadBlockFromField(initializer_list<TextureRect> rects, const string& id) {
    struct ThemeDir { CourseTheme theme; const char* dir; };
    static const ThemeDir dirs[] = {
        {Ground, "ground"},
        {Underground, "underground"},
        {Underwater, "water"},
        {Desert, "desert"},
        {Snow, "snow"},
        {Sky, "sky"},
        {Forest, "forest"},
        {GhostHouse, "ghost_house"},
        {Airship, "airship"},
        {Castle, "castle"}
    };

    for (const auto& td : dirs) {
        string base = "assets/textures/SMB1/" + string(td.dir) + "/";
        if (FileExists((base + "field.png").c_str())) {
            for (const auto& rect : rects) {
                loadBlockFromSheet(id, {SMB1, td.theme, Day}, base + "field.png", rect);
            }
        }
        if (FileExists((base + "field_M.png").c_str())) {
            for (const auto& rect : rects) {
                loadBlockFromSheet(id, {SMB1, td.theme, Night}, base + "field_M.png", rect);
            }
        }
    }
}

BlockPos getOffsetPos(BlockPos pos,int x,int y) {
    return BlockPos{pos.x + x, pos.y + y};
}

set<int> getBlockOnPos(BlockPos pos) {
    set<int> result;
    for (int ind = 0,end = static_cast<int>(levelBlocks.size()); ind < end; ind++) {
        if (levelBlocks[ind].pos == pos) result.insert(ind);
    }
    return result;
}

template<typename Container, typename Func>
bool checkValid(const Container& x, Func func) {
    for (const auto& i : x) {
        if (func(i)) return true;
    }
    return false;
}

bool checkHaveBlock(const set<int> &blocksInd, const string &id) {
    for (const int ind : blocksInd) {
        if (levelBlocks[ind].id == id) return true;
    }
    return false;
}

int getBlockTextureIndex(int blockIndex) {
    const string& id = levelBlocks[blockIndex].id;
    if (currentCourseInfo.style == SMB1) {
        if (currentCourseInfo.theme == Ground) return 0;
        if (currentCourseInfo.theme == Underground) return 0;
        if (currentCourseInfo.theme == Underwater) {
            if (id == "ground") {
                const set<int> down = getBlockOnPos(getOffsetPos(levelBlocks[blockIndex].pos, 0, 1));
                const set<int> left = getBlockOnPos(getOffsetPos(levelBlocks[blockIndex].pos, -1, 0));
                const set<int> right = getBlockOnPos(getOffsetPos(levelBlocks[blockIndex].pos, 1, 0));
                if ((!checkHaveBlock(down,"ground"))
                || (checkHaveBlock(left,"ground"))
                || (checkHaveBlock(right,"ground"))) return 0;
                return 1;
            }
            return 0;
        }
    }
    return -1;
}

void DrawPlayerShadow(WorldPos pos) {
    PlayerTexture2DInfo key = {currentCourseInfo.style, currentPlayer, currentAbility};
    auto it = playerTextures.find(key);
    if (it == playerTextures.end() || it->second.empty()) return;
    auto& vec = it->second;
    Texture2D tex = vec[isDead ? playerFrames["dead0"] : playerAnimFrame];
    Rectangle src = playerFacingRight ?
        (Rectangle){0, 0, static_cast<float>(tex.width), static_cast<float>(tex.height)} :
        (Rectangle){static_cast<float>(tex.width), 0, -static_cast<float>(tex.width), static_cast<float>(tex.height)};
    ScreenPos screen = {pos.x + cameraX, pos.y + cameraY};
    Rectangle shadowDest = {screen.x + shadowOffset, screen.y + shadowOffset,
                            static_cast<float>(tex.width) * SCALE, static_cast<float>(tex.height) * SCALE};
    DrawTexturePro(tex, src, shadowDest, (Vector2){0, 0}, 0.0f, Fade(BLACK, 0.5f));
}

void DrawPlayer(WorldPos pos) {
    PlayerTexture2DInfo key = {currentCourseInfo.style, currentPlayer, currentAbility};
    auto it = playerTextures.find(key);
    if (it == playerTextures.end() || it->second.empty()) return;
    auto& vec = it->second;
    Texture2D tex = vec[isDead ? playerFrames["dead0"] : playerAnimFrame];
    Rectangle src = playerFacingRight ?
        (Rectangle){0, 0, static_cast<float>(tex.width), static_cast<float>(tex.height)} :
        (Rectangle){static_cast<float>(tex.width), 0, -static_cast<float>(tex.width), static_cast<float>(tex.height)};
    ScreenPos screen = {pos.x + cameraX, pos.y + cameraY};
    Rectangle dest = {screen.x, screen.y,
                      static_cast<float>(tex.width) * SCALE, static_cast<float>(tex.height) * SCALE};
    DrawTexturePro(tex, src, dest, (Vector2){0, 0}, 0.0f, WHITE);
}

void DrawBlockShadow(int blockIndex) {
    const PlacedBlock& block = levelBlocks[blockIndex];
    const BlockTexture2DInfo key = {currentCourseInfo, block.id};
    const auto it = blockTextures.find(key);
    if (it == blockTextures.end() || it->second.empty()) return;
    int idx = getBlockTextureIndex(blockIndex);
    if (idx < 0 || idx >= static_cast<int>(it->second.size())) return;
    const auto px = static_cast<float>((block.pos.x - 1) * BLOCK_PX) + cameraX;
    const auto py = static_cast<float>((block.pos.y - 1) * BLOCK_PX) + cameraY;
    DrawTextureEx(it->second[idx], (Vector2){px + shadowOffset, py + shadowOffset}, 0.0f, static_cast<float>(SCALE), Fade(BLACK, 0.3f));
}

void DrawBlock(int blockIndex) {
    const PlacedBlock& block = levelBlocks[blockIndex];
    const BlockTexture2DInfo key = {currentCourseInfo, block.id};
    const auto it = blockTextures.find(key);
    if (it == blockTextures.end() || it->second.empty()) return;
    int idx = getBlockTextureIndex(blockIndex);
    if (idx < 0 || idx >= static_cast<int>(it->second.size())) return;
    const auto px = static_cast<float>((block.pos.x - 1) * BLOCK_PX) + cameraX;
    const auto py = static_cast<float>((block.pos.y - 1) * BLOCK_PX) + cameraY;
    DrawTextureEx(it->second[idx], (Vector2){px, py}, 0.0f, static_cast<float>(SCALE), WHITE);
}

void DrawBlockWithFrameIndex(const BlockPos pos, const string &id, const int ind) {
    const BlockTexture2DInfo key = {currentCourseInfo, id};
    const auto it = blockTextures.find(key);
    if (it == blockTextures.end() || it->second.empty()) return;
    if (ind < 0 || ind >= static_cast<int>(it->second.size())) return;
    const auto px = static_cast<float>((pos.x - 1) * BLOCK_PX) + cameraX;
    const auto py = static_cast<float>((pos.y - 1) * BLOCK_PX) + cameraY;
    DrawTextureEx(it->second[ind], (Vector2){px, py}, 0.0f, static_cast<float>(SCALE), WHITE);
}

void DrawBlockWithTexture(const BlockPos pos, const Texture2D &tex) {
    const auto px = static_cast<float>((pos.x - 1) * BLOCK_PX) + cameraX;
    const auto py = static_cast<float>((pos.y - 1) * BLOCK_PX) + cameraY;
    DrawTextureEx(tex, (Vector2){px, py}, 0.0f, static_cast<float>(SCALE), WHITE);
}

int main() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "SMMRX - Super Mario Maker RX");
    SetExitKey(KEY_NULL);
    ClearWindowState(FLAG_WINDOW_RESIZABLE);
    SetTargetFPS(60);
    loadLanguages();
    initFont();
    InitAudioDevice();
    loadSoundEffectsFromCSV("assets/data/ses.csv");

    loadBlockFromField({{144, 112}}, "ground");
    loadBlockFromField({{16, 0}}, "brick_block");
    loadBlockFromField({{96, 0}}, "hard_block");
    loadBlocksFromCSV("assets/data/blocks.csv");
    loadBackgroundsFromCSV("assets/data/backgrounds.csv");
    loadPlayersFromCSV("assets/data/players.csv");
    loadBGMLoopsFromCSV("assets/data/bgm_loop.csv");
    loadBGMFromTheme(SMB1, Ground);
    loadBGMFromTheme(SMB1, Underground);
    loadBGMFromTheme(SMB1, Underwater);
    hurryUpSe = LoadMusicStream("assets/musics/SMB1/Hurry.mp3");
    loadGuiFromCSV("assets/data/guis.csv");
    hud.init();

    initLevel();

    float startTimer = 0.0f;
    bool startClickable = false;

    float animTimer = 0.0f;


    bool onGround = false;
    bool isCrouching = false;
    bool crouchJump = false;
    bool isBraking = false;
    float brakeTimer = 0.0f;

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        if (hasCurrentBGM) {
            UpdateMusicStream(currentBGM);
            musicLogicalPos += dt;
            if (currentBGMHasLoop) {
                float pos = GetMusicTimePlayed(currentBGM);
                if (pos >= currentBGMLoopEnd) {
                    float overflow = pos - currentBGMLoopEnd;
                    SeekMusicStream(currentBGM, currentBGMLoopStart + overflow);
                }
            }
        }
        if (playingHurryUp) {
            UpdateMusicStream(hurryUpSe);
            hurryUpTimer += dt;
            if (hurryUpTimer >= 3.1f) {
                playingHurryUp = false;
                StopMusicStream(hurryUpSe);
                playBGM(hurryUpPendingStyle, hurryUpPendingTheme, hurryUpPendingType);
            }
        }
        updateBGMFade(dt);
        updateBackgroundAnim(dt);
        int timeBefore = static_cast<int>(currentTime);
        if (state == STATE_GAME && !isDead) hud.update(dt);
        int timeAfter = static_cast<int>(currentTime);
        if (state == STATE_GAME && !isDead && currentTime <= 0.0f) {
            die();
        }
        if (!hurryUpTriggered && timeBefore > 99 && timeAfter <= 99 && state == STATE_GAME) {
            hurryUpTriggered = true;
            stopBGM();
            PlayMusicStream(hurryUpSe);
            SetMusicVolume(hurryUpSe, bgmVolume * 0.5f);
            playingHurryUp = true;
            hurryUpTimer = 0.0f;
            hurryUpPendingStyle = SMB1;
            hurryUpPendingTheme = currentCourseInfo.theme;
            hurryUpPendingType = (currentCourseInfo.time == Night) ? PlayMoonHurry : PlayHurry;
        }

        switch (state) {
            case STATE_START:
                startTimer += dt;
                if (startTimer >= 2.0f) startClickable = true;
                if (startClickable && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) state = STATE_ANIMATION;
                break;
            case STATE_ANIMATION:
                animTimer += dt;
                if (animTimer >= animDuration) {
                    state = STATE_GAME;
                    musicLogicalPos = 0.0f;
                    gameOver = false;
                    isDead = false;
                    hurryUpTriggered = false;
                    if (currentTime <= 100) {
                        hurryUpTriggered = true;
                        PlayMusicStream(hurryUpSe);
                        SetMusicVolume(hurryUpSe, bgmVolume * 0.5f);
                        playingHurryUp = true;
                        hurryUpTimer = 0.0f;
                        hurryUpPendingStyle = SMB1;
                        hurryUpPendingTheme = currentCourseInfo.theme;
                        hurryUpPendingType = (currentCourseInfo.time == Night) ? PlayMoonHurry : PlayHurry;
                    } else {
                        BGMType bgmType = (currentCourseInfo.time == Night) ? PlayMoon : PlayNormal;
                        playBGM(SMB1, currentCourseInfo.theme, bgmType);
                    }
                    hud.start();
                }
                break;
            case STATE_GAME: {
                if (IsKeyPressed(KEY_F1)) debugMode = !debugMode;
                if (!gameOver) {
                constexpr float camSpeed = 1000.0f;
                isCrouching = (onGround || crouchJump) && IsKeyDown(KEY_S);
                bool groundCrouch = isCrouching && onGround;
                float currentMaxSpeed = IsMouseButtonDown(MOUSE_BUTTON_LEFT) ? playerSprintMaxSpeed : playerMaxSpeed;
                if (!isBraking && onGround && fabs(playerXSpeed) >= playerSprintMaxSpeed * 0.9f) {
                    if ((playerXSpeed > 0 && IsKeyDown(KEY_A)) || (playerXSpeed < 0 && IsKeyDown(KEY_D))) {
                        isBraking = true;
                        brakeTimer = 0.0f;
                    }
                }
                if (isBraking) {
                    if (fabs(playerXSpeed) > 1.0f) {
                        if (playerXSpeed > 0) playerXSpeed = max(0.0f, playerXSpeed - playerTurnFriction * dt);
                        else playerXSpeed = min(0.0f, playerXSpeed + playerTurnFriction * dt);
                    } else {
                        playerXSpeed = 0.0f;
                        brakeTimer += dt;
                        if (brakeTimer >= playerTurnDelay) isBraking = false;
                    }
                } else if (!groundCrouch) {
                    if (IsKeyDown(KEY_A) && playerXSpeed > -currentMaxSpeed) {
                        playerXSpeed -= playerAccel * dt;
                        if (playerXSpeed < -currentMaxSpeed) playerXSpeed = -currentMaxSpeed;
                    }
                    if (IsKeyDown(KEY_D) && playerXSpeed < currentMaxSpeed) {
                        playerXSpeed += playerAccel * dt;
                        if (playerXSpeed > currentMaxSpeed) playerXSpeed = currentMaxSpeed;
                    }
                }
                bool overSpeed = fabs(playerXSpeed) > currentMaxSpeed + 1.0f;
                if (!isBraking && (groundCrouch || (!IsKeyDown(KEY_A) && !IsKeyDown(KEY_D)) || overSpeed)) {
                    if (playerXSpeed > 0) playerXSpeed = max(0.0f, playerXSpeed - playerFriction * dt);
                    else if (playerXSpeed < 0) playerXSpeed = min(0.0f, playerXSpeed + playerFriction * dt);
                }
                float prevX = playerPos.x;
                playerPos.x += playerXSpeed * dt;
                playerBox.x = playerPos.x + playerBoxInset;
                playerBox.width = BLOCK_PX - playerBoxInset * 2.0f;
                float boxHeight = BLOCK_PX - playerBoxTopInset;
                if (isCrouching) boxHeight *= 0.5f;
                playerBox.height = boxHeight;
                playerBox.y = playerPos.y + BLOCK_PX - playerBox.height;
                for (const auto& block : levelBlocks) {
                    CollisionBox blockBox = getBlockBox(block.pos, block.id);
                    if (boxOverlap(playerBox, blockBox)) {
                        float prevBoxRight = prevX + playerBoxInset + playerBox.width;
                        float prevBoxLeft = prevX + playerBoxInset;
                        if (playerXSpeed > 0 && prevBoxRight <= blockBox.x + 1.0f) {
                            playerPos.x = blockBox.x - playerBox.width - playerBoxInset;
                            playerXSpeed = 0.0f;
                            playerBox.x = playerPos.x + playerBoxInset;
                        } else if (playerXSpeed < 0 && prevBoxLeft >= blockBox.x + blockBox.width - 1.0f) {
                            playerPos.x = blockBox.x + blockBox.width - playerBoxInset;
                            playerXSpeed = 0.0f;
                            playerBox.x = playerPos.x + playerBoxInset;
                        }
                    }
                }
                playerYSpeed += playerGravity * dt;
                if (playerYSpeed > playerMaxFallSpeed) playerYSpeed = playerMaxFallSpeed;
                if (IsKeyPressed(KEY_SPACE) && onGround) {
                    float speedRatio = fabs(playerXSpeed) / currentMaxSpeed;
                    if (speedRatio > 1.0f) speedRatio = 1.0f;
                    playerYSpeed = -(playerJumpSpeedMin + speedRatio * (playerJumpSpeedMax - playerJumpSpeedMin));
                    onGround = false;
                    if (isCrouching) crouchJump = true;
                    playSoundEffect("player.small_jump");
                }
                float prevBoxY = playerBox.y;
                playerPos.y += playerYSpeed * dt;
                playerBox.y = playerPos.y + BLOCK_PX - playerBox.height;
                onGround = false;
                for (const auto& block : levelBlocks) {
                    CollisionBox blockBox = getBlockBox(block.pos, block.id);
                    if (boxOverlap(playerBox, blockBox)) {
                        if (playerYSpeed > 0 && prevBoxY + playerBox.height <= blockBox.y + 1.0f) {
                            playerPos.y = blockBox.y - BLOCK_PX;
                            playerYSpeed = 0.0f;
                            onGround = true;
                            crouchJump = false;
                            playerBox.y = playerPos.y + BLOCK_PX - playerBox.height;
                        } else if (playerYSpeed < 0 && prevBoxY >= blockBox.y + blockBox.height - 1.0f) {
                            playerPos.y = blockBox.y + blockBox.height - (BLOCK_PX - playerBox.height);
                            playerYSpeed = 0.0f;
                            playerBox.y = playerPos.y + BLOCK_PX - playerBox.height;
                        }
                    }
                }
                if (onGround) {
                    if (isBraking) {
                        playerAnimFrame = playerFrames["turn0"];
                    } else if (groundCrouch) {
                        if (IsKeyDown(KEY_D)) playerFacingRight = true;
                        else if (IsKeyDown(KEY_A)) playerFacingRight = false;
                        playerAnimFrame = playerFrames["stoop0"];
                    } else {
                        if (playerXSpeed > 10.0f) playerFacingRight = true;
                        else if (playerXSpeed < -10.0f) playerFacingRight = false;
                        if (fabs(playerXSpeed) < 10.0f) {
                            playerAnimFrame = playerFrames["stand0"];
                        } else {
                        playerAnimTimer += dt * (fabs(playerXSpeed) / 100.0f) * playerWalkAnimSpeed;
                        playerAnimFrame = playerFrames["walk0"] + (static_cast<int>(playerAnimTimer) % 3);
                        }
                    }
                } else {
                    if (crouchJump) playerAnimFrame = playerFrames["stoop0"];
                    else if (playerYSpeed < 0) playerAnimFrame = playerFrames["jump0"];
                    else playerAnimFrame = playerFrames["jump0"];
                }
                cameraX = SCREEN_WIDTH / 2.0f - BLOCK_PX / 2.0f - playerPos.x;
                float playerScreenY = playerPos.y + cameraY;
                float deadTop = SCREEN_HEIGHT / 3.0f;
                float deadBottom = SCREEN_HEIGHT * 2.0f / 3.0f;
                if (playerScreenY < deadTop) {
                    cameraY = deadTop - playerPos.y;
                } else if (playerScreenY > deadBottom) {
                    cameraY = deadBottom - playerPos.y;
                }
                // if (IsKeyDown(KEY_LEFT))  cameraX += camSpeed * dt;
                // if (IsKeyDown(KEY_RIGHT)) cameraX -= camSpeed * dt;
                // if (IsKeyDown(KEY_UP))    cameraY += camSpeed * dt;
                // if (IsKeyDown(KEY_DOWN))  cameraY -= camSpeed * dt;
                constexpr float camMinX = -(COURSE_WIDTH - SCREEN_WIDTH);
                constexpr float camMaxX = 0;
                constexpr float camMinY = -(COURSE_HEIGHT - SCREEN_HEIGHT);
                constexpr float camMaxY = 0;
#if ENABLE_CAMERA_BOUNDS
                if (cameraX < camMinX) cameraX = camMinX;
                if (cameraX > camMaxX) cameraX = camMaxX;
                if (cameraY < camMinY) cameraY = camMinY;
                if (cameraY > camMaxY) cameraY = camMaxY;
#endif
                }
                if (playerPos.y > COURSE_HEIGHT + BLOCK_PX) die(false);
                break;
            }
            case STATE_DEAD:
                if (!deathJumped) {
                    deathWaitTimer += dt;
                    if (deathWaitTimer >= playerDeathWaitTime) {
                        deathJumped = true;
                        if (deathBounce) playerYSpeed = -playerDeathJumpSpeed;
                    }
                } else {
                    playerYSpeed += playerGravity * dt;
                    playerPos.y += playerYSpeed * dt;
                }
                break;
        }

        BeginDrawing();

        switch (state) {
            case STATE_START: {
                ClearBackground(RAYWHITE);
                const char* title = langTextC("gui.title");
                const char* sub = langTextC("gui.subtitle");
                int tSize = 80, sSize = 30;
                int tw = measureText(title, tSize);
                int sw = measureText(sub, sSize);
                drawText(title, (SCREEN_WIDTH - tw) / 2, SCREEN_HEIGHT / 2 - 60, tSize, DARKGRAY);
                drawText(sub, (SCREEN_WIDTH - sw) / 2, SCREEN_HEIGHT / 2 + 30, sSize, GRAY);
                if (startClickable) {
                    const char* hint = langTextC("gui.click_to_start");
                    int hw = measureText(hint, 24);
                    drawText(hint, (SCREEN_WIDTH - hw) / 2, SCREEN_HEIGHT - 100, 24, LIGHTGRAY);
                }
                break;
            }
            case STATE_ANIMATION: {
                ClearBackground(BLACK);
                float a = animTimer < 1.0f ? animTimer :
                          (animTimer > animDuration - 1.0f ? animDuration - animTimer : 1.0f);
                const char* animText = langTextC("gui.intro_animation"); drawText(animText, (SCREEN_WIDTH - measureText(animText, 40)) / 2, SCREEN_HEIGHT / 2 - 20, 40, Fade(WHITE, a));
                break;
            }
            case STATE_DEAD:
            case STATE_GAME:
                ClearBackground(SKYBLUE);
                DrawBackground();
                for (int i = 0; i < static_cast<int>(levelBlocks.size()); i++) DrawBlockShadow(i);
                DrawPlayerShadow(playerPos);
                for (int i = 0; i < static_cast<int>(levelBlocks.size()); i++) DrawBlock(i);
                DrawPlayer(playerPos);
                if (debugMode) {
                    drawText(TextFormat(langTextC("debug.speed"), fabs(playerXSpeed) / BLOCK_PX), 10, 10, 40, WHITE);
                }
                hud.draw();
                break;
        }

        EndDrawing();
    }

    for (auto& [info, vec] : backgrounds)
        for (auto& tex : vec)
            UnloadTexture(tex);
    for (auto& [info, vec] : blockTextures)
        for (auto& tex : vec)
            UnloadTexture(tex);
    for (auto& [info, vec] : playerTextures)
        for (auto& tex : vec)
            UnloadTexture(tex);
    for (auto& [info, data] : bgms)
        UnloadMusicStream(data.music);
    for (auto& [info, vec] : guiTextures)
        for (auto& tex : vec)
            UnloadTexture(tex);
    hud.unload();
    CloseAudioDevice();
    CloseWindow();
    return 0;
}
