#ifndef GAME_HPP
#define GAME_HPP

#include <raylib.h>
#include "GameContext.hpp"
#include "Renderer.hpp"
#include "../entities/EntityPlayer.hpp"
#include "../ui/HudManager.hpp"
#include "../settings/animation_settings.hpp"

class Game {
public:
    GameContext ctx;
    Renderer renderer;
    EntityPlayer player;
    HudManager hud;

    void init();
    void run();
    void close();

private:
    void initLevel();
    void updateStart(float dt);
    void updateAnimation(float dt);
    void updateGame(float dt);
    void renderStart();
    void renderAnimation();
    void renderGame();
    void renderBackground();
    void updateBackgroundAnim(float dt);
};

#endif
