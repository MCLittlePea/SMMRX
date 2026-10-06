#include "BlockSpikeTrap.hpp"
#include <cmath>
#include "../render/TextureManager.hpp"
#include "../settings/render_settings.hpp"

constexpr float spikeBlinkNormalTime = 1.0f;
constexpr float spikeBlinkDimTime = 0.3f;
constexpr Color spikeDimColor = {160, 160, 160, 255};

void BlockSpikeTrap::render(const GameContext& ctx) const {
    const BlockTexture2DInfo key = {ctx.currentCourseInfo, id};
    const auto it = TextureManager::blockTextures.find(key);
    if (it == TextureManager::blockTextures.end() || it->second.empty()) return;
    int idx = getTextureIndex(ctx);
    if (idx < 0 || idx >= static_cast<int>(it->second.size())) return;
    const auto px = static_cast<float>((pos.x - 1) * BLOCK_PX) + ctx.cameraX;
    const auto py = static_cast<float>((pos.y - 1) * BLOCK_PX) + ctx.cameraY;
    float cycle = spikeBlinkNormalTime + spikeBlinkDimTime;
    float t = fmodf(GetTime(), cycle);
    Color tint = (t < spikeBlinkNormalTime) ? WHITE : spikeDimColor;
    DrawTextureEx(it->second[idx], (Vector2){px, py}, 0.0f, key.scale, tint);
}
