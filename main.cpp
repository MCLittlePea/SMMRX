#include <raylib.h>
#include <cmath>
#include <map>
#include <set>
#include <string>
#include <algorithm>
#include <vector>
#include <fstream>
#include <sstream>

using namespace std;

enum GameState {
    STATE_START,
    STATE_ANIMATION,
    STATE_GAME
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
    Nothing,
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

    bool operator<(const BlockTexture2DInfo& o) const {
        if (info != o.info) return info < o.info;
        return id < o.id;
    }
};

struct PlayerTexture2DInfo {
    GameStyle style;
    Character player;
    Ability ability;

    bool operator<(const PlayerTexture2DInfo& o) const {
        if (style != o.style) return style < o.style;
        if (player != o.player) return player < o.player;
        return ability < o.ability;
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

    bool operator<(const GuiTextureInfo& o) const {
        if (style != o.style) return style < o.style;
        return type < o.type;
    }
};

Character currentPlayer = MARIO;
Ability currentAbility = Nothing;
float currentTime;
float playerStateTimer = 0.0f;
int playerAnimFrame = 0;
float playerAnimTimer = 0.0f;
bool playerFacingRight = true;
CourseInfo currentCourseInfo = {SMB1, Underwater, Night, 1000};

map<BlockTexture2DInfo, vector<Texture2D>> blockTextures;
map<PlayerTexture2DInfo, vector<Texture2D>> playerTextures;
map<BGMInfo, Music> bgms;
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
map<CourseInfo, vector<Texture2D>> backgrounds;
int bgAnimFrame = 0;
float bgAnimTimer = 0.0f;
constexpr float bgAnimFrameDuration = 0.5f;

vector<PlacedBlock> levelBlocks;

void addBlock(BlockPos pos, const string& id) {
    levelBlocks.push_back({pos, id});
}

void initLevel() {
    for (int i = 1; i <= 72; i++) addBlock({i, 28}, "ground");
    for (int i = 1; i <= 72; i++) addBlock({i, 27}, "ground");
    addBlock({5,26}, "ground");
    addBlock({12, 24}, "brick_block");
    addBlock({13, 24}, "hard_block");
}

constexpr int TILE_SIZE = 16;
constexpr int SCALE = 4;
constexpr int BLOCK_PX = TILE_SIZE * SCALE;
constexpr int SCREEN_WIDTH = BLOCK_PX * 24;
constexpr int SCREEN_HEIGHT = BLOCK_PX * 27 / 2;
constexpr int COURSE_WIDTH = 72 * BLOCK_PX;
constexpr int COURSE_HEIGHT = 28 * BLOCK_PX;



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

WorldPos playerPos = {(3 - 1) * BLOCK_PX, (26 - 1) * BLOCK_PX};

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

void loadBGM(GameStyle style, CourseTheme theme, BGMType type, const string& path) {
    Music m = LoadMusicStream(path.c_str());
    bgms[{style, theme, type}] = m;
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

void playBGM(GameStyle style, CourseTheme theme, BGMType type) {
    auto it = bgms.find({style, theme, type});
    if (it == bgms.end()) return;
    if (hasCurrentBGM) StopMusicStream(currentBGM);
    currentBGM = it->second;
    PlayMusicStream(currentBGM);
    SetMusicVolume(currentBGM, bgmVolume);
    hasCurrentBGM = true;
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
    return Nothing;
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
    constexpr float bgSize = 512.0f * SCALE;
    constexpr float bgY = -2 * BLOCK_PX;
    constexpr float bgOffsetX = -8 * BLOCK_PX;
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

void DrawPlayer(WorldPos pos) {
    PlayerTexture2DInfo key = {currentCourseInfo.style, currentPlayer, currentAbility};
    auto it = playerTextures.find(key);
    if (it == playerTextures.end() || it->second.empty()) return;
    auto& vec = it->second;
    Texture2D tex = vec[playerAnimFrame % static_cast<int>(vec.size())];
    Rectangle src = playerFacingRight ?
        (Rectangle){0, 0, static_cast<float>(tex.width), static_cast<float>(tex.height)} :
        (Rectangle){static_cast<float>(tex.width), 0, -static_cast<float>(tex.width), static_cast<float>(tex.height)};
    ScreenPos screen = {pos.x + cameraX, pos.y + cameraY};
    Rectangle dest = {screen.x, screen.y,
                      static_cast<float>(tex.width) * SCALE, static_cast<float>(tex.height) * SCALE};
    DrawTexturePro(tex, src, dest, (Vector2){0, 0}, 0.0f, WHITE);
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
    ClearWindowState(FLAG_WINDOW_RESIZABLE);
    SetTargetFPS(60);
    InitAudioDevice();

    loadBlockFromField({{144, 112}}, "ground");
    loadBlockFromField({{16, 0}}, "brick_block");
    loadBlockFromField({{96, 0}}, "hard_block");
    loadBlocksFromCSV("assets/data/blocks.csv");
    loadBackgroundsFromCSV("assets/data/backgrounds.csv");
    loadPlayersFromCSV("assets/data/players.csv");
    loadBGMFromTheme(SMB1, Ground);
    loadBGMFromTheme(SMB1, Underground);
    loadBGMFromTheme(SMB1, Underwater);
    hurryUpSe = LoadMusicStream("assets/musics/SMB1/Hurry.mp3");
    loadGuiFromCSV("assets/data/guis.csv");
    hud.init();

    initLevel();

    GameState state = STATE_START;
    float startTimer = 0.0f;
    bool startClickable = false;

    float animTimer = 0.0f;
    constexpr float animDuration = 3.0f;

    float playerXSpeed = 0;
    constexpr float playerMaxSpeed = 400.0f;
    constexpr float playerAccel = 2000.0f;
    constexpr float playerFriction = 1500.0f; // 10.8 block per seconds

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        if (hasCurrentBGM) UpdateMusicStream(currentBGM);
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
        hud.update(dt);
        int timeAfter = static_cast<int>(currentTime);
        if (!hurryUpTriggered && timeBefore > 100 && timeAfter <= 100 && state == STATE_GAME) {
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
                constexpr float camSpeed = 1000.0f;
                if (IsKeyDown(KEY_A)) playerXSpeed -= playerAccel * dt;
                if (IsKeyDown(KEY_D)) playerXSpeed += playerAccel * dt;
                if (!IsKeyDown(KEY_A) && !IsKeyDown(KEY_D)) {
                    if (playerXSpeed > 0) playerXSpeed = max(0.0f, playerXSpeed - playerFriction * dt);
                    else if (playerXSpeed < 0) playerXSpeed = min(0.0f, playerXSpeed + playerFriction * dt);
                }
                playerXSpeed = max(playerXSpeed, -playerMaxSpeed);
                playerXSpeed = min(playerXSpeed, playerMaxSpeed);
                playerPos.x += playerXSpeed * dt;
                cameraX = SCREEN_WIDTH / 2.0f - BLOCK_PX / 2.0f - playerPos.x;
                cameraY = SCREEN_HEIGHT / 2.0f - BLOCK_PX / 2.0f - playerPos.y;
                // if (IsKeyDown(KEY_LEFT))  cameraX += camSpeed * dt;
                // if (IsKeyDown(KEY_RIGHT)) cameraX -= camSpeed * dt;
                // if (IsKeyDown(KEY_UP))    cameraY += camSpeed * dt;
                // if (IsKeyDown(KEY_DOWN))  cameraY -= camSpeed * dt;
                constexpr float camMinX = -(COURSE_WIDTH - SCREEN_WIDTH);
                constexpr float camMaxX = 0;
                constexpr float camMinY = -(COURSE_HEIGHT - SCREEN_HEIGHT);
                constexpr float camMaxY = 0;
                if (cameraX < camMinX) cameraX = camMinX;
                if (cameraX > camMaxX) cameraX = camMaxX;
                if (cameraY < camMinY) cameraY = camMinY;
                if (cameraY > camMaxY) cameraY = camMaxY;
                break;
            }
        }

        BeginDrawing();

        switch (state) {
            case STATE_START: {
                ClearBackground(RAYWHITE);
                const char* title = "SMMRX";
                const char* sub = "Super Mario Maker RX";
                int tSize = 80, sSize = 30;
                int tw = MeasureText(title, tSize);
                int sw = MeasureText(sub, sSize);
                DrawText(title, (SCREEN_WIDTH - tw) / 2, SCREEN_HEIGHT / 2 - 60, tSize, DARKGRAY);
                DrawText(sub, (SCREEN_WIDTH - sw) / 2, SCREEN_HEIGHT / 2 + 30, sSize, GRAY);
                if (startClickable) {
                    const char* hint = "Click anywhere to start";
                    int hw = MeasureText(hint, 24);
                    DrawText(hint, (SCREEN_WIDTH - hw) / 2, SCREEN_HEIGHT - 100, 24, LIGHTGRAY);
                }
                break;
            }
            case STATE_ANIMATION: {
                ClearBackground(BLACK);
                float a = animTimer < 1.0f ? animTimer :
                          (animTimer > animDuration - 1.0f ? animDuration - animTimer : 1.0f);
                DrawText("Intro Animation", SCREEN_WIDTH / 2 - 200, SCREEN_HEIGHT / 2 - 20, 40, Fade(WHITE, a));
                break;
            }
            case STATE_GAME:
                ClearBackground(SKYBLUE);
                DrawBackground();
                for (int i = 0; i < static_cast<int>(levelBlocks.size()); i++) DrawBlock(i);
                DrawRectangle(
                    static_cast<int>(playerPos.x + cameraX),
                    static_cast<int>(playerPos.y + cameraY),
                    BLOCK_PX, BLOCK_PX, BLACK);
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
    for (auto& [info, m] : bgms)
        UnloadMusicStream(m);
    for (auto& [info, vec] : guiTextures)
        for (auto& tex : vec)
            UnloadTexture(tex);
    hud.unload();
    CloseAudioDevice();
    CloseWindow();
    return 0;
}
