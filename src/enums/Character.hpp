#ifndef ENUMS_CHARACTER_HPP
#define ENUMS_CHARACTER_HPP

#include <map>
#include <string>

using namespace std;

enum Character {
    MARIO,
    LUIGI,
    TOAD,
    TOADETTE,
    CHAR_ERROR
};

class CharacterInfo {
public:
    static const map<string, Character> strMap;
    static Character parse(const string& s);
};

#endif
