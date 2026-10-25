#include "BlockGoalPoleBase.hpp"
#include <cmath>

void BlockGoalPoleBase::render(const GameContext& ctx) const {
    auto& gp = TextureManager::goalpoleTextures;
    if (gp.find("base") == gp.end()) return;
    float px = floorf((float)(pos.x - 1) * BLOCK_PX + ctx.cameraX);
    float py = floorf((float)(pos.y - 1) * BLOCK_PX + ctx.cameraY);
    DrawTextureEx(gp["base"], (Vector2){px, py}, 0.0f, static_cast<float>(SCALE), WHITE);
}

void BlockGoalPoleBase::renderShadow(const GameContext& ctx) const {
    auto& gp = TextureManager::goalpoleTextures;
    if (gp.find("base") == gp.end()) return;
    float px = floorf((float)(pos.x - 1) * BLOCK_PX + ctx.cameraX) + shadowOffset;
    float py = floorf((float)(pos.y - 1) * BLOCK_PX + ctx.cameraY) + shadowOffset;
    DrawTextureEx(gp["base"], (Vector2){px, py}, 0.0f, static_cast<float>(SCALE), Fade(BLACK, 0.3f));
}

