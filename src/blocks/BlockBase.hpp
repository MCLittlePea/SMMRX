#ifndef BLOCK_BASE_HPP
#define BLOCK_BASE_HPP

#include <raylib.h>
#include <string>
#include "../types.hpp"
#include "../core/GameContext.hpp"
#include "../render/TextureManager.hpp"
#include "../settings/render_settings.hpp"

using namespace std;

class BlockBase {
public:
    BlockPos pos;
    string id;

    BlockBase(BlockPos p, const string& i) : pos(p), id(i) {}
    virtual ~BlockBase() = default;

    virtual void update(float dt, GameContext& ctx) {}
    virtual void render(const GameContext& ctx) const;
    virtual void renderShadow(const GameContext& ctx) const;
    virtual CollisionBox getCollisionBox() const;
    virtual bool isSolid() const { return true; }

protected:
    virtual int getTextureIndex(const GameContext& ctx) const;
};

#endif
