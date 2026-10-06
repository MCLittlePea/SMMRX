#ifndef BLOCK_GROUND_HPP
#define BLOCK_GROUND_HPP

#include "BlockBase.hpp"

class BlockGround : public BlockBase {
public:
    BlockGround(BlockPos p) : BlockBase(p, "ground") {}
};

#endif
