#ifndef ENUMS_ABILITY_HPP
#define ENUMS_ABILITY_HPP

#include <map>
#include <string>

using namespace std;

enum Ability {
    Small,
    Super,
    Fire,
    Big,
    SMB2,
    Link,
    SuperBall,
    Racoon,
    Frog,
    Cape,
    Balloon,
    Propeller,
    FlyingSquirrel,
    Cat,
    Boomerang,
    Builder,
    ABILITY_ERROR
};

class AbilityInfo {
public:
    static const map<string, Ability> strMap;
    static Ability parse(const string& s);
};

#endif
