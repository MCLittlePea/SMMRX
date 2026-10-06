#include "HudManager.hpp"
#include "../settings/gui_settings.hpp"
#include <string>

using namespace std;

void HudManager::init(GameContext& ctx) {
    ctx.currentTime = ctx.currentCourseInfo.maxTime;
}

void HudManager::start() {
    running = true;
}

void HudManager::update(float dt, GameContext& ctx) {
    if (running && ctx.currentTime > 0) {
        ctx.currentTime -= dt;
        if (ctx.currentTime < 0) ctx.currentTime = 0;
    }
}

void HudManager::drawGlyph(const string& type, int index, float x, float y, const CourseInfo& info) {
    GuiTextureInfo key = {info.style, type};
    auto it = TextureManager::guiTextures.find(key);
    if (it == TextureManager::guiTextures.end() || index >= static_cast<int>(it->second.size())) return;
    Texture2D glyph = it->second[index];
    if (glyph.id == 0) return;
    DrawTextureEx(glyph, {x, y}, 0.0f, key.scale, WHITE);
}

void HudManager::draw(const GameContext& ctx) {
    int total = static_cast<int>(ctx.currentTime);
    if (total > 9999) total = 9999;
    string total_s = to_string(total);
    while (total_s.size() < 4) total_s = "0" + total_s;
    constexpr float glyphW = hudGlyphWidth * guiScale;
    constexpr float glyphGap = hudGlyphGap * guiScale;
    constexpr float marginX = hudMarginX * guiScale;
    constexpr float marginY = hudMarginY * guiScale;
    float x = SCREEN_WIDTH - glyphW * 5.0f - marginX;
    float y = marginY;
    drawGlyph("NUMBER_FONT", 10, x, y, ctx.currentCourseInfo);
    drawGlyph("NUMBER_FONT", total_s[0] - '0', x + glyphW + glyphGap, y, ctx.currentCourseInfo);
    drawGlyph("NUMBER_FONT", total_s[1] - '0', x + glyphW * 2 + glyphGap, y, ctx.currentCourseInfo);
    drawGlyph("NUMBER_FONT", total_s[2] - '0', x + glyphW * 3 + glyphGap, y, ctx.currentCourseInfo);
    drawGlyph("NUMBER_FONT", total_s[3] - '0', x + glyphW * 4 + glyphGap, y, ctx.currentCourseInfo);
}

void HudManager::unload() {}
