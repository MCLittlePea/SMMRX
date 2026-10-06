#ifndef BLOCK_SPIKE_TRAP_HPP
#define BLOCK_SPIKE_TRAP_HPP

#include "BlockBase.hpp"

class BlockSpikeTrap : public BlockBase {
public:
    BlockSpikeTrap(BlockPos p) : BlockBase(p, "spike_trap") {}
    void render(const GameContext& ctx) const override;
};

#endif
