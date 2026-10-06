#include "BlockRegistry.hpp"

map<string, BlockRegistry::FactoryFn>& BlockRegistry::getRegistry() {
    static map<string, FactoryFn> registry;
    return registry;
}

bool BlockRegistry::registerBlock(const string& id, FactoryFn fn) {
    getRegistry()[id] = fn;
    return true;
}

BlockBase* BlockRegistry::create(const string& id, BlockPos pos) {
    auto it = getRegistry().find(id);
    if (it == getRegistry().end()) return nullptr;
    return it->second(pos);
}

bool BlockRegistry::hasBlock(const string& id) {
    return getRegistry().find(id) != getRegistry().end();
}
