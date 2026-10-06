#ifndef TEXTURE_MANAGER_HPP
#define TEXTURE_MANAGER_HPP

#include <raylib.h>
#include <map>
#include <vector>
#include <string>
#include "../types.hpp"

using namespace std;

class TextureManager {
public:
    static map<BlockTexture2DInfo, vector<Texture2D>> blockTextures;
    static map<PlayerTexture2DInfo, vector<Texture2D>> playerTextures;
    static map<GuiTextureInfo, vector<Texture2D>> guiTextures;
    static map<CourseInfo, vector<Texture2D>> backgrounds;

    static void initBlock(const BlockTexture2DInfo& info, const Texture2D& tex);
    static void loadBlockFromSheet(const string& id, CourseInfo info, const string& path, TextureRect rect);
    static void loadPlayerFrames(const string& style, const string& player, const string& ability, const string& path, int x, int y, int count, int frameW, int frameH);
    static void loadBackground(CourseInfo info, const string& path);
    static void loadGuiFromSheet(const string& type, const string& style, const string& path, TextureRect rect);
    static void loadBlockFromField(initializer_list<TextureRect> rects, const string& id);
    static void loadBlocksFromCSV(const string& path);
    static void loadBackgroundsFromCSV(const string& path);
    static void loadPlayersFromCSV(const string& path);
    static void loadGuiFromCSV(const string& path);
    static void unloadAll();
};

#endif
