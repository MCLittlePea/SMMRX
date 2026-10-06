#ifndef HUD_MANAGER_HPP
#define HUD_MANAGER_HPP

#include <raylib.h>
#include "../types.hpp"
#include "../core/GameContext.hpp"
#include "../render/TextureManager.hpp"

class HudManager {
public:
    bool running = false;

    void init(GameContext& ctx);
    void start();
    void update(float dt, GameContext& ctx);
    void drawGlyph(const string& type, int index, float x, float y, const CourseInfo& info);
    void draw(const GameContext& ctx);
    void unload();
};

#endif
