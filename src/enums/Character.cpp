#include "Character.hpp"

const map<string, Character> CharacterInfo::strMap = {
    {"MARIO", MARIO}, {"LUIGI", LUIGI}, {"TOAD", TOAD}, {"TOADETTE", TOADETTE}
};

Character CharacterInfo::parse(const string& s) {
    auto it = strMap.find(s);
    return it != strMap.end() ? it->second : CHAR_ERROR;
}
