#ifndef TYPES_HPP
#define TYPES_HPP

#define ENABLE_CAMERA_BOUNDS 1
#define ENABLE_ANIMATION 1

#include <raylib.h>
#include <string>
#include "enums.hpp"
#include "settings/render_settings.hpp"

using namespace std;

struct BlockPos {
    int x;
    int y;

    bool operator<(const BlockPos& o) const {
        if (x != o.x) return x < o.x;
        return y < o.y;
    }
    bool operator==(const BlockPos& o) const {
        if (x != o.x) return false;
        if (y != o.y) return false;
        return true;
    }
};

struct PlacedBlock {
    BlockPos pos;
    string id;
};

struct WorldPos {
    float x;
    float y;
};

struct ScreenPos {
    float x;
    float y;
};

struct TextureRect {
    int x{};
    int y{};
    int width = 16;
    int height = 16;
};

struct CourseInfo {
    string style;
    string theme;
    string time;
    int maxTime = 300;

    bool operator<(const CourseInfo& o) const {
        if (style != o.style) return style < o.style;
        if (theme != o.theme) return theme < o.theme;
        return time < o.time;
    }

    bool operator==(const CourseInfo& o) const {
        if (style != o.style) return false;
        if (theme != o.theme) return false;
        return time == o.time;
    }
};

struct CollisionBox {
    float x, y, width, height;
};

struct BlockTexture2DInfo {
    CourseInfo info;
    string id;
    float scale = 16.0f;

    bool operator<(const BlockTexture2DInfo& o) const {
        if (info != o.info) return info < o.info;
        if (id != o.id) return id < o.id;
        return scale < o.scale;
    }
};

struct PlayerTexture2DInfo {
    string style;
    string player;
    string ability;
    float scale = 16.0f;

    bool operator<(const PlayerTexture2DInfo& o) const {
        if (style != o.style) return style < o.style;
        if (player != o.player) return player < o.player;
        if (ability != o.ability) return ability < o.ability;
        return scale < o.scale;
    }
};

struct BGMInfo {
    string style;
    string theme;
    string type;

    bool operator<(const BGMInfo& o) const {
        if (style != o.style) return style < o.style;
        if (theme != o.theme) return theme < o.theme;
        return type < o.type;
    }
};

struct GuiTextureInfo {
    string style;
    string type;
    float scale = 4.0f;

    bool operator<(const GuiTextureInfo& o) const {
        if (style != o.style) return style < o.style;
        if (type != o.type) return type < o.type;
        return scale < o.scale;
    }
};

#endif
