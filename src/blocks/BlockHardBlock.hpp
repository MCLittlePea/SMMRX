#ifndef BLOCK_HARD_BLOCK_HPP
#define BLOCK_HARD_BLOCK_HPP

#include "BlockBase.hpp"

class BlockHardBlock : public BlockBase {
public:
    BlockHardBlock(BlockPos p) : BlockBase(p, "hard_block") {}
};

#endif
