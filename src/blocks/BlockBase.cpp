#include "BlockBase.hpp"
#include <set>
#include <algorithm>

static map<string, pair<float, float>> blockBoxSizes = {
    {"ground", {1.0f, 1.0f}},
    {"block", {1.0f, 1.0f}},
    {"hard_block", {1.0f, 1.0f}},
    {"spike_trap", {1.0f, 1.0f}},
};

static BlockPos getOffsetPos(BlockPos pos, int x, int y) {
    return BlockPos{pos.x + x, pos.y + y};
}

static set<int> getBlockOnPos(BlockPos pos, const vector<PlacedBlock>& blocks) {
    set<int> result;
    for (int ind = 0, end = static_cast<int>(blocks.size()); ind < end; ind++) {
        if (blocks[ind].pos == pos) result.insert(ind);
    }
    return result;
}

static bool checkHaveBlock(const set<int>& blocksInd, const string& id, const vector<PlacedBlock>& blocks) {
    for (const int ind : blocksInd) {
        if (blocks[ind].id == id) return true;
    }
    return false;
}

int BlockBase::getTextureIndex(const GameContext& ctx) const {
    if (ctx.currentCourseInfo.style == "SMB1") {
        if (ctx.currentCourseInfo.theme == "Ground") return 0;
        if (ctx.currentCourseInfo.theme == "Underground") return 0;
        if (ctx.currentCourseInfo.theme == "Underwater") {
            if (id == "ground") {
                const set<int> down = getBlockOnPos(getOffsetPos(pos, 0, 1), ctx.levelBlocks);
                const set<int> left = getBlockOnPos(getOffsetPos(pos, -1, 0), ctx.levelBlocks);
                const set<int> right = getBlockOnPos(getOffsetPos(pos, 1, 0), ctx.levelBlocks);
                if ((!checkHaveBlock(down, "ground", ctx.levelBlocks))
                || (checkHaveBlock(left, "ground", ctx.levelBlocks))
                || (checkHaveBlock(right, "ground", ctx.levelBlocks))) return 0;
                return 1;
            }
            return 0;
        }
    }
    return -1;
}

CollisionBox BlockBase::getCollisionBox() const {
    auto it = blockBoxSizes.find(id);
    float w = (it != blockBoxSizes.end()) ? it->second.first : 1.0f;
    float h = (it != blockBoxSizes.end()) ? it->second.second : 1.0f;
    return {(float)(pos.x - 1) * BLOCK_PX, (float)(pos.y - 1) * BLOCK_PX, (float)BLOCK_PX * w, (float)BLOCK_PX * h};
}

void BlockBase::render(const GameContext& ctx) const {
    const BlockTexture2DInfo key = {ctx.currentCourseInfo, id};
    const auto it = TextureManager::blockTextures.find(key);
    if (it == TextureManager::blockTextures.end() || it->second.empty()) return;
    int idx = getTextureIndex(ctx);
    if (idx < 0 || idx >= static_cast<int>(it->second.size())) return;
    const auto px = static_cast<float>((pos.x - 1) * BLOCK_PX) + ctx.cameraX;
    const auto py = static_cast<float>((pos.y - 1) * BLOCK_PX) + ctx.cameraY;
    DrawTextureEx(it->second[idx], (Vector2){px, py}, 0.0f, key.scale, WHITE);
}

void BlockBase::renderShadow(const GameContext& ctx) const {
    const BlockTexture2DInfo key = {ctx.currentCourseInfo, id};
    const auto it = TextureManager::blockTextures.find(key);
    if (it == TextureManager::blockTextures.end() || it->second.empty()) return;
    int idx = getTextureIndex(ctx);
    if (idx < 0 || idx >= static_cast<int>(it->second.size())) return;
    const auto px = static_cast<float>((pos.x - 1) * BLOCK_PX) + ctx.cameraX;
    const auto py = static_cast<float>((pos.y - 1) * BLOCK_PX) + ctx.cameraY;
    DrawTextureEx(it->second[idx], (Vector2){px + shadowOffset, py + shadowOffset}, 0.0f, key.scale, Fade(BLACK, 0.3f));
}
