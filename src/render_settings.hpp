#ifndef RENDER_SETTINGS_HPP
#define RENDER_SETTINGS_HPP

constexpr int TILE_SIZE = 16;
constexpr int SCALE = 4;
constexpr int BLOCK_PX = TILE_SIZE * SCALE;

constexpr float shadowOffset = 12.0f;

constexpr float bgAnimFrameDuration = 0.5f;
constexpr float bgSize = 512.0f * SCALE;
constexpr float bgY = -2 * BLOCK_PX;
constexpr float bgOffsetX = -8 * BLOCK_PX;

#endif
