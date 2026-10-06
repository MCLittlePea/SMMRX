#include "TextureManager.hpp"
#include <fstream>
#include <sstream>
#include "../settings/render_settings.hpp"
#include "../enums.hpp"

map<BlockTexture2DInfo, vector<Texture2D>> TextureManager::blockTextures;
map<PlayerTexture2DInfo, vector<Texture2D>> TextureManager::playerTextures;
map<GuiTextureInfo, vector<Texture2D>> TextureManager::guiTextures;
map<CourseInfo, vector<Texture2D>> TextureManager::backgrounds;

static vector<string> splitCSV(const string& line) {
    vector<string> result;
    stringstream ss(line);
    string cell;
    while (getline(ss, cell, ',')) result.push_back(cell);
    return result;
}

void TextureManager::initBlock(const BlockTexture2DInfo& info, const Texture2D& tex) {
    blockTextures[info].push_back(tex);
}

void TextureManager::loadBlockFromSheet(const string& id, CourseInfo info, const string& path, TextureRect rect) {
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

void TextureManager::loadPlayerFrames(const string& style, const string& player, const string& ability, const string& path, int x, int y, int count, int frameW, int frameH) {
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

void TextureManager::loadBackground(CourseInfo info, const string& path) {
    Texture2D tex = LoadTexture(path.c_str());
    SetTextureFilter(tex, TEXTURE_FILTER_POINT);
    backgrounds[info].push_back(tex);
}

void TextureManager::loadGuiFromSheet(const string& type, const string& style, const string& path, TextureRect rect) {
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

void TextureManager::loadBlockFromField(initializer_list<TextureRect> rects, const string& id) {
    struct ThemeDir { string theme; const char* dir; };
    static const ThemeDir dirs[] = {
        {"Ground", "ground"}, {"Underground", "underground"}, {"Underwater", "water"},
        {"Desert", "desert"}, {"Snow", "snow"}, {"Sky", "sky"},
        {"Forest", "forest"}, {"GhostHouse", "ghost_house"}, {"Airship", "airship"}, {"Castle", "castle"}
    };
    for (const auto& td : dirs) {
        string base = "assets/textures/SMB1/" + string(td.dir) + "/";
        if (FileExists((base + "field.png").c_str())) {
            for (const auto& rect : rects) {
                loadBlockFromSheet(id, {"SMB1", td.theme, "Day"}, base + "field.png", rect);
            }
        }
        if (FileExists((base + "field_M.png").c_str())) {
            for (const auto& rect : rects) {
                loadBlockFromSheet(id, {"SMB1", td.theme, "Night"}, base + "field_M.png", rect);
            }
        }
    }
}

void TextureManager::loadBlocksFromCSV(const string& path) {
    ifstream file(path);
    string line;
    getline(file, line);
    while (getline(file, line)) {
        if (line.empty()) continue;
        auto c = splitCSV(line);
        if (c.size() < 9) continue;
        loadBlockFromSheet(c[0], {c[1], c[2], c[3]},
                            c[4], {stoi(c[5]), stoi(c[6]), stoi(c[7]), stoi(c[8])});
    }
}

void TextureManager::loadBackgroundsFromCSV(const string& path) {
    ifstream file(path);
    string line;
    getline(file, line);
    while (getline(file, line)) {
        if (line.empty()) continue;
        auto c = splitCSV(line);
        if (c.size() < 4) continue;
        loadBackground({c[0], c[1], c[2]}, c[3]);
    }
}

void TextureManager::loadPlayersFromCSV(const string& path) {
    ifstream file(path);
    string line;
    getline(file, line);
    while (getline(file, line)) {
        if (line.empty()) continue;
        auto c = splitCSV(line);
        if (c.size() < 9) continue;
        loadPlayerFrames(c[0], c[1], c[2],
                          c[3], stoi(c[4]), stoi(c[5]), stoi(c[6]), stoi(c[7]), stoi(c[8]));
    }
}

void TextureManager::loadGuiFromCSV(const string& path) {
    ifstream file(path);
    string line;
    getline(file, line);
    while (getline(file, line)) {
        if (line.empty()) continue;
        auto c = splitCSV(line);
        if (c.size() < 7) continue;
        loadGuiFromSheet(c[0], c[1],
                          c[2], {stoi(c[3]), stoi(c[4]), stoi(c[5]), stoi(c[6])});
    }
}

void TextureManager::unloadAll() {
    for (auto& [info, vec] : backgrounds)
        for (auto& tex : vec) UnloadTexture(tex);
    for (auto& [info, vec] : blockTextures)
        for (auto& tex : vec) UnloadTexture(tex);
    for (auto& [info, vec] : playerTextures)
        for (auto& tex : vec) UnloadTexture(tex);
    for (auto& [info, vec] : guiTextures)
        for (auto& tex : vec) UnloadTexture(tex);
}
