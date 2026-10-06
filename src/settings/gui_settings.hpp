#ifndef GUI_SETTINGS_HPP
#define GUI_SETTINGS_HPP

#include "render_settings.hpp"

constexpr float guiScale = static_cast<float>(SCALE) / 4.0f;

constexpr int startTitleSize = 80;
constexpr int startSubtitleSize = 30;
constexpr int startHintSize = 24;
constexpr float startTitleOffsetY = 60.0f;
constexpr float startSubtitleOffsetY = 30.0f;
constexpr float startHintOffsetBottom = 100.0f;

constexpr int animTextSize = 40;
constexpr float animTextOffsetY = 20.0f;

constexpr int debugTextSize = 40;
constexpr float debugTextOffsetX = 10.0f;
constexpr float debugTextOffsetY = 10.0f;

constexpr float hudGlyphWidth = 35.0f;
constexpr float hudGlyphGap = 10.0f;
constexpr float hudMarginX = 50.0f;
constexpr float hudMarginY = 32.0f;

#endif
