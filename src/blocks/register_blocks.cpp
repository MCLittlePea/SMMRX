#include "BlockRegistry.hpp"
#include "BlockGround.hpp"
#include "BlockBlock.hpp"
#include "BlockHardBlock.hpp"
#include "BlockSpikeTrap.hpp"
#include "BlockGoalPoleBase.hpp"

REGISTER_BLOCK("ground", BlockGround);
REGISTER_BLOCK("block", BlockBlock);
REGISTER_BLOCK("hard_block", BlockHardBlock);
REGISTER_BLOCK("spike_trap", BlockSpikeTrap);
REGISTER_BLOCK("goal_pole_base", BlockGoalPoleBase);
