#ifndef GAME_CONTEXT_HPP
#define GAME_CONTEXT_HPP

#include <raylib.h>
#include <map>
#include <vector>
#include <string>
#include "../types.hpp"
#include "../enums.hpp"
#include "../settings/render_settings.hpp"
#include "../settings/window_settings.hpp"
#include "../settings/course_settings.hpp"

using namespace std;

struct GameContext {
    CourseInfo currentCourseInfo = {"SMB1", "Ground", "Day", 1000};
    vector<PlacedBlock> levelBlocks;
    WorldPos playerPos = {(3 - 1) * BLOCK_PX, (5 - 1) * BLOCK_PX};
    string currentPlayer = "MARIO";
    string currentAbility = "Small";
    bool characterDifference = false;

    float playerXSpeed = 0.0f;
    float playerYSpeed = 0.0f;
    bool playerFacingRight = true;
    int playerAnimFrame = 0;
    float playerAnimTimer = 0.0f;
    map<string, int> playerFrames = {
        {"climb0", 0}, {"climb1", 1}, {"dead0", 2}, {"jump0", 3},
        {"stand0", 4}, {"stoop0", 5}, {"swim0", 6}, {"swim1", 7},
        {"swim2", 8}, {"swim3", 9}, {"swim4", 10}, {"swim5", 11},
        {"turn0", 12}, {"walk0", 13}, {"walk1", 14}, {"walk2", 15}
    };
    CollisionBox playerBox = {};

    bool onGround = false;
    bool isCrouching = false;
    bool crouchJump = false;
    bool isTurning = false;
    float turnTimer = 0.0f;

    bool isDead = false;
    float deathWaitTimer = 0.0f;
    bool deathJumped = false;
    bool deathBounce = true;

    string state = "START";
    bool gameOver = false;
    bool debugMode = false;
    float currentTime = 0.0f;

    int bgAnimFrame = 0;
    float bgAnimTimer = 0.0f;

    float cameraX = 0.0f;
    float cameraY = -(COURSE_HEIGHT - SCREEN_HEIGHT);

    float startTimer = 0.0f;
    bool startClickable = false;
    float animTimer = 0.0f;
};

#endif
