#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <raylib.h>
#include "../types.hpp"
#include "../core/GameContext.hpp"
#include "../render/TextureManager.hpp"
#include "../audio/AudioManager.hpp"
#include "../settings/player_settings.hpp"
#include "../settings/render_settings.hpp"
#include "../settings/course_settings.hpp"

class EntityPlayer {
public:
    void update(float dt, GameContext& ctx);
    void render(const GameContext& ctx) const;
    void renderShadow(const GameContext& ctx) const;
    void die(GameContext& ctx, bool bounce = true);
    void updateDeath(float dt, GameContext& ctx);

private:
    void updateMovement(float dt, GameContext& ctx);
    void updatePhysics(float dt, GameContext& ctx);
    void updateAnimation(float dt, GameContext& ctx);
    void updateCamera(GameContext& ctx);
    void resolveHorizontalCollision(GameContext& ctx, float prevX);
    void resolveVerticalCollision(GameContext& ctx, float prevBoxY);
};

#endif
