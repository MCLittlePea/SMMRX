#include "Registry.hpp"
#include "Meta.hpp"

void registerAll() {
    Registry<GameStyleMeta>::registerItem("SMB1", {"SMB1", "Super Mario Bros."});
    Registry<GameStyleMeta>::registerItem("SMB3", {"SMB3", "Super Mario Bros. 3"});
    Registry<GameStyleMeta>::registerItem("SMW", {"SMW", "Super Mario World"});
    Registry<GameStyleMeta>::registerItem("NSMBU", {"NSMBU", "New Super Mario Bros. U"});
    Registry<GameStyleMeta>::registerItem("SM3DW", {"SM3DW", "Super Mario 3D World"});

    Registry<CharacterMeta>::registerItem("MARIO", {"MARIO", "Mario"});
    Registry<CharacterMeta>::registerItem("LUIGI", {"LUIGI", "Luigi"});
    Registry<CharacterMeta>::registerItem("TOAD", {"TOAD", "Toad"});
    Registry<CharacterMeta>::registerItem("TOADETTE", {"TOADETTE", "Toadette"});

    Registry<CourseThemeMeta>::registerItem("Ground", {"Ground", "Ground"});
    Registry<CourseThemeMeta>::registerItem("Underground", {"Underground", "Underground"});
    Registry<CourseThemeMeta>::registerItem("Underwater", {"Underwater", "Underwater"});
    Registry<CourseThemeMeta>::registerItem("Desert", {"Desert", "Desert"});
    Registry<CourseThemeMeta>::registerItem("Snow", {"Snow", "Snow"});
    Registry<CourseThemeMeta>::registerItem("Sky", {"Sky", "Sky"});
    Registry<CourseThemeMeta>::registerItem("Forest", {"Forest", "Forest"});
    Registry<CourseThemeMeta>::registerItem("GhostHouse", {"GhostHouse", "Ghost House"});
    Registry<CourseThemeMeta>::registerItem("Airship", {"Airship", "Airship"});
    Registry<CourseThemeMeta>::registerItem("Castle", {"Castle", "Castle"});

    Registry<TimeMeta>::registerItem("Day", {"Day", "Day"});
    Registry<TimeMeta>::registerItem("Night", {"Night", "Night"});

    Registry<BGMTypeMeta>::registerItem("Edit", {"Edit", "Edit"});
    Registry<BGMTypeMeta>::registerItem("PlayNormal", {"PlayNormal", "Normal"});
    Registry<BGMTypeMeta>::registerItem("PlayMoon", {"PlayMoon", "Moon"});
    Registry<BGMTypeMeta>::registerItem("PlayHurry", {"PlayHurry", "Hurry"});
    Registry<BGMTypeMeta>::registerItem("PlayMoonHurry", {"PlayMoonHurry", "Moon Hurry"});
    Registry<BGMTypeMeta>::registerItem("Hurry", {"Hurry", "Hurry Jingle"});

    Registry<GuiElementTypeMeta>::registerItem("NUMBER_FONT", {"NUMBER_FONT", "Number Font"});

    Registry<AbilityMeta>::registerItem("Small", {"Small", "Small"});
    Registry<AbilityMeta>::registerItem("Super", {"Super", "Super"});
    Registry<AbilityMeta>::registerItem("Fire", {"Fire", "Fire"});
    Registry<AbilityMeta>::registerItem("Big", {"Big", "Big"});
    Registry<AbilityMeta>::registerItem("SMB2", {"SMB2", "SMB2"});
    Registry<AbilityMeta>::registerItem("Link", {"Link", "Link"});
    Registry<AbilityMeta>::registerItem("SuperBall", {"SuperBall", "Super Ball"});
    Registry<AbilityMeta>::registerItem("Racoon", {"Racoon", "Racoon"});
    Registry<AbilityMeta>::registerItem("Frog", {"Frog", "Frog"});
    Registry<AbilityMeta>::registerItem("Cape", {"Cape", "Cape"});
    Registry<AbilityMeta>::registerItem("Balloon", {"Balloon", "Balloon"});
    Registry<AbilityMeta>::registerItem("Propeller", {"Propeller", "Propeller"});
    Registry<AbilityMeta>::registerItem("FlyingSquirrel", {"FlyingSquirrel", "Flying Squirrel"});
    Registry<AbilityMeta>::registerItem("Cat", {"Cat", "Cat"});
    Registry<AbilityMeta>::registerItem("Boomerang", {"Boomerang", "Boomerang"});
    Registry<AbilityMeta>::registerItem("Builder", {"Builder", "Builder"});

    Registry<GameStateMeta>::registerItem("START", {"START", "Start"});
    Registry<GameStateMeta>::registerItem("ANIMATION", {"ANIMATION", "Animation"});
    Registry<GameStateMeta>::registerItem("GAME", {"GAME", "Game"});
    Registry<GameStateMeta>::registerItem("DEAD", {"DEAD", "Dead"});

    Registry<LanguageMeta>::registerItem("English", {"English", "English"});
    Registry<LanguageMeta>::registerItem("Chinese", {"Chinese", "Chinese"});
}
