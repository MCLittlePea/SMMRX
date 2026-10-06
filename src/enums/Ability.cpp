#include "Ability.hpp"

const map<string, Ability> AbilityInfo::strMap = {
    {"Small", Small}, {"Nothing", Small}, {"Super", Super}, {"Fire", Fire}, {"Big", Big},
    {"SMB2", SMB2}, {"Link", Link}, {"SuperBall", SuperBall}, {"Racoon", Racoon},
    {"Frog", Frog}, {"Cape", Cape}, {"Balloon", Balloon}, {"Propeller", Propeller},
    {"FlyingSquirrel", FlyingSquirrel}, {"Cat", Cat}, {"Boomerang", Boomerang},
    {"Builder", Builder}
};

Ability AbilityInfo::parse(const string& s) {
    auto it = strMap.find(s);
    return it != strMap.end() ? it->second : ABILITY_ERROR;
}
