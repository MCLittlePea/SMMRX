#ifndef BLOCK_REGISTRY_HPP
#define BLOCK_REGISTRY_HPP

#include <map>
#include <string>
#include <functional>
#include "BlockBase.hpp"

using namespace std;

class BlockRegistry {
public:
    using FactoryFn = function<BlockBase*(BlockPos)>;

    static bool registerBlock(const string& id, FactoryFn fn);
    static BlockBase* create(const string& id, BlockPos pos);
    static bool hasBlock(const string& id);

private:
    static map<string, FactoryFn>& getRegistry();
};

#define REGISTER_BLOCK(id, cls) \
    static bool _registered_##cls = BlockRegistry::registerBlock(id, \
        [](BlockPos pos) -> BlockBase* { return new cls(pos); })

#endif
