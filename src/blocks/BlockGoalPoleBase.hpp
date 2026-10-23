#ifndef BLOCK_GOAL_POLE_BASE_HPP
#define BLOCK_GOAL_POLE_BASE_HPP

#include "BlockBase.hpp"

class BlockGoalPoleBase : public BlockBase {
public:
    BlockGoalPoleBase(BlockPos p) : BlockBase(p, "goal_pole_base") {}
    void render(const GameContext& ctx) const override;
    void renderShadow(const GameContext& ctx) const override;
};

#endif
