#ifndef BLOCK_BLOCK_HPP
#define BLOCK_BLOCK_HPP

#include "BlockBase.hpp"

class BlockBlock : public BlockBase {
public:
    BlockBlock(BlockPos p) : BlockBase(p, "block") {}
};

#endif
