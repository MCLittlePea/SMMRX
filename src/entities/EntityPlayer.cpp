#include "EntityPlayer.hpp"
#include <cmath>
#include <algorithm>

using namespace std;

static bool boxOverlap(const CollisionBox& a, const CollisionBox& b) {
    return a.x < b.x + b.width && a.x + a.width > b.x &&
           a.y < b.y + b.height && a.y + a.height > b.y;
}

void EntityPlayer::update(float dt, GameContext& ctx) {
    if (ctx.isDead) {
        updateDeath(dt, ctx);
        return;
    }
    if (ctx.gameOver) return;
    if (IsKeyPressed(KEY_F1)) ctx.debugMode = !ctx.debugMode;

    updateMovement(dt, ctx);
    updatePhysics(dt, ctx);
    updateAnimation(dt, ctx);
    updateCamera(ctx);

    if (ctx.playerPos.y > COURSE_HEIGHT + BLOCK_PX) die(ctx, false);
}

void EntityPlayer::updateMovement(float dt, GameContext& ctx) {
    float effMaxSpeed = playerMaxSpeed;
    float effSprintMaxSpeed = playerSprintMaxSpeed;
    float effAccel = playerAccel;
    float effFriction = playerFriction;
    float effGravity = playerGravity;
    float effMaxFallSpeed = playerMaxFallSpeed;
    float effJumpMin = playerJumpSpeedMin;
    float effJumpMax = playerJumpSpeedMax;
    float effBoxInset = playerBoxInset;
    float effBoxTopInset = playerBoxTopInset;
    if (ctx.characterDifference) {
        if (ctx.currentPlayer == "LUIGI") {
            effJumpMin *= luigiJumpMultiplier;
            effJumpMax *= luigiJumpMultiplier;
            effGravity *= luigiGravityMultiplier;
            effMaxFallSpeed *= luigiFallSpeedMultiplier;
        } else if (ctx.currentPlayer == "TOAD") {
            effMaxSpeed *= toadSpeedMultiplier;
            effSprintMaxSpeed *= toadSpeedMultiplier;
            effAccel *= toadAccelMultiplier;
            effFriction *= toadFrictionMultiplier;
        } else if (ctx.currentPlayer == "TOADETTE") {
            effBoxInset += toadetteBoxInsetBonus;
            effBoxTopInset += toadetteBoxTopInsetBonus;
        }
    }

    ctx.isCrouching = (ctx.onGround || ctx.crouchJump) && IsKeyDown(KEY_S);
    bool groundCrouch = ctx.isCrouching && ctx.onGround;
    float currentMaxSpeed = IsMouseButtonDown(MOUSE_BUTTON_LEFT) ? effSprintMaxSpeed : effMaxSpeed;

    if (!ctx.isTurning && ctx.onGround && fabs(ctx.playerXSpeed) > playerMaxSpeed) {
        if ((ctx.playerXSpeed > 0 && IsKeyDown(KEY_A)) || (ctx.playerXSpeed < 0 && IsKeyDown(KEY_D))) {
            ctx.isTurning = true;
            ctx.turnTimer = 0.0f;
        }
    } else if (!ctx.isTurning && ctx.onGround) {
        if ((ctx.playerXSpeed > 0 && IsKeyDown(KEY_A)) || (ctx.playerXSpeed < 0 && IsKeyDown(KEY_D))) {
            ctx.playerXSpeed = 0;
        }
    }
    if (ctx.isTurning) {
        if (fabs(ctx.playerXSpeed) > 1.0f) {
            if (ctx.playerXSpeed > 0) ctx.playerXSpeed = max(0.0f, ctx.playerXSpeed - playerTurnFriction * dt);
            else ctx.playerXSpeed = min(0.0f, ctx.playerXSpeed + playerTurnFriction * dt);
        } else {
            ctx.playerXSpeed = 0.0f;
            ctx.turnTimer += dt;
            if (ctx.turnTimer >= playerTurnDelay) ctx.isTurning = false;
        }
    } else if (!groundCrouch) {
        if (IsKeyDown(KEY_A) && ctx.playerXSpeed > -currentMaxSpeed) {
            ctx.playerXSpeed -= effAccel * dt;
            if (ctx.playerXSpeed < -currentMaxSpeed) ctx.playerXSpeed = -currentMaxSpeed;
        }
        if (IsKeyDown(KEY_D) && ctx.playerXSpeed < currentMaxSpeed) {
            ctx.playerXSpeed += effAccel * dt;
            if (ctx.playerXSpeed > currentMaxSpeed) ctx.playerXSpeed = currentMaxSpeed;
        }
    }
    bool overSpeed = fabs(ctx.playerXSpeed) > currentMaxSpeed + 1.0f;
    if (!ctx.isTurning && (groundCrouch || (!IsKeyDown(KEY_A) && !IsKeyDown(KEY_D)) || overSpeed)) {
        if (ctx.playerXSpeed > 0) ctx.playerXSpeed = max(0.0f, ctx.playerXSpeed - effFriction * dt);
        else if (ctx.playerXSpeed < 0) ctx.playerXSpeed = min(0.0f, ctx.playerXSpeed + effFriction * dt);
    }

    float prevX = ctx.playerPos.x;
    ctx.playerPos.x += ctx.playerXSpeed * dt;
    ctx.playerBox.x = ctx.playerPos.x + effBoxInset;
    ctx.playerBox.width = BLOCK_PX - effBoxInset * 2.0f;
    float boxHeight = BLOCK_PX - effBoxTopInset;
    if (ctx.isCrouching) boxHeight *= 0.5f;
    ctx.playerBox.height = boxHeight;
    ctx.playerBox.y = ctx.playerPos.y + BLOCK_PX - ctx.playerBox.height;
    resolveHorizontalCollision(ctx, prevX);

    ctx.playerYSpeed += effGravity * dt;
    if (ctx.playerYSpeed > effMaxFallSpeed) ctx.playerYSpeed = effMaxFallSpeed;
    if (IsKeyPressed(KEY_SPACE) && ctx.onGround) {
        float speedRatio = fabs(ctx.playerXSpeed) / currentMaxSpeed;
        if (speedRatio > 1.0f) speedRatio = 1.0f;
        ctx.playerYSpeed = -(effJumpMin + speedRatio * (effJumpMax - effJumpMin));
        ctx.onGround = false;
        if (ctx.isCrouching) ctx.crouchJump = true;
        AudioManager::playSoundEffect("player.small_jump");
    }
    float prevBoxY = ctx.playerBox.y;
    ctx.playerPos.y += ctx.playerYSpeed * dt;
    ctx.playerBox.y = ctx.playerPos.y + BLOCK_PX - ctx.playerBox.height;
    resolveVerticalCollision(ctx, prevBoxY);
}

void EntityPlayer::resolveHorizontalCollision(GameContext& ctx, float prevX) {
    float effBoxInset = playerBoxInset;
    if (ctx.characterDifference && ctx.currentPlayer == "TOADETTE") {
        effBoxInset += toadetteBoxInsetBonus;
    }
    for (const auto& block : ctx.levelBlocks) {
        CollisionBox blockBox = {(float)(block.pos.x - 1) * BLOCK_PX, (float)(block.pos.y - 1) * BLOCK_PX, (float)BLOCK_PX, (float)BLOCK_PX};
        if (boxOverlap(ctx.playerBox, blockBox)) {
            if (block.id == "spike_trap") { die(ctx); return; }
            float prevBoxLeft = prevX + effBoxInset;
            float prevBoxRight = prevBoxLeft + ctx.playerBox.width;
            if (ctx.playerXSpeed > 0 && prevBoxRight <= blockBox.x + 1.0f) {
                ctx.playerPos.x = blockBox.x - ctx.playerBox.width - effBoxInset;
                ctx.playerXSpeed = 0.0f;
                ctx.playerBox.x = ctx.playerPos.x + effBoxInset;
            } else if (ctx.playerXSpeed < 0 && prevBoxLeft >= blockBox.x + blockBox.width - 1.0f) {
                ctx.playerPos.x = blockBox.x + blockBox.width - effBoxInset;
                ctx.playerXSpeed = 0.0f;
                ctx.playerBox.x = ctx.playerPos.x + effBoxInset;
            }
        }
    }
}

void EntityPlayer::resolveVerticalCollision(GameContext& ctx, float prevBoxY) {
    ctx.onGround = false;
    for (const auto& block : ctx.levelBlocks) {
        CollisionBox blockBox = {(float)(block.pos.x - 1) * BLOCK_PX, (float)(block.pos.y - 1) * BLOCK_PX, (float)BLOCK_PX, (float)BLOCK_PX};
        if (boxOverlap(ctx.playerBox, blockBox)) {
            if (block.id == "spike_trap") { die(ctx); return; }
            if (ctx.playerYSpeed > 0 && prevBoxY + ctx.playerBox.height <= blockBox.y + 1.0f) {
                ctx.playerPos.y = blockBox.y - BLOCK_PX;
                ctx.playerYSpeed = 0.0f;
                ctx.onGround = true;
                ctx.crouchJump = false;
                ctx.playerBox.y = ctx.playerPos.y + BLOCK_PX - ctx.playerBox.height;
            } else if (ctx.playerYSpeed < 0 && prevBoxY >= blockBox.y + blockBox.height - 1.0f) {
                ctx.playerPos.y = blockBox.y + blockBox.height - (BLOCK_PX - ctx.playerBox.height);
                ctx.playerYSpeed = 0.0f;
                ctx.playerBox.y = ctx.playerPos.y + BLOCK_PX - ctx.playerBox.height;
            }
        }
    }
}

void EntityPlayer::updatePhysics(float dt, GameContext& ctx) {
}

void EntityPlayer::updateAnimation(float dt, GameContext& ctx) {
    if (ctx.onGround) {
        if (ctx.isTurning) {
            ctx.playerAnimFrame = ctx.playerFrames["turn0"];
        } else if (ctx.isCrouching && ctx.onGround) {
            if (IsKeyDown(KEY_D)) ctx.playerFacingRight = true;
            else if (IsKeyDown(KEY_A)) ctx.playerFacingRight = false;
            ctx.playerAnimFrame = ctx.playerFrames["stoop0"];
        } else {
            if (ctx.playerXSpeed > 10.0f) ctx.playerFacingRight = true;
            else if (ctx.playerXSpeed < -10.0f) ctx.playerFacingRight = false;
            if (fabs(ctx.playerXSpeed) < 10.0f) {
                ctx.playerAnimFrame = ctx.playerFrames["stand0"];
            } else {
                ctx.playerAnimTimer += dt * (fabs(ctx.playerXSpeed) / 100.0f) * playerWalkAnimSpeed;
                ctx.playerAnimFrame = ctx.playerFrames["walk0"] + (static_cast<int>(ctx.playerAnimTimer) % 3);
            }
        }
    } else {
        if (ctx.crouchJump) ctx.playerAnimFrame = ctx.playerFrames["stoop0"];
        else if (ctx.playerYSpeed < 0) ctx.playerAnimFrame = ctx.playerFrames["jump0"];
        else ctx.playerAnimFrame = ctx.playerFrames["jump0"];
    }
}

void EntityPlayer::updateCamera(GameContext& ctx) {
    ctx.cameraX = SCREEN_WIDTH / 2.0f - BLOCK_PX / 2.0f - ctx.playerPos.x;
    float playerScreenY = ctx.playerPos.y + ctx.cameraY;
    float deadTop = SCREEN_HEIGHT / 3.0f;
    float deadBottom = SCREEN_HEIGHT * 2.0f / 3.0f;
    if (playerScreenY < deadTop) {
        ctx.cameraY = deadTop - ctx.playerPos.y;
    } else if (playerScreenY > deadBottom) {
        ctx.cameraY = deadBottom - ctx.playerPos.y;
    }
#if ENABLE_CAMERA_BOUNDS
    constexpr float camMinX = -(COURSE_WIDTH - SCREEN_WIDTH);
    constexpr float camMaxX = 0;
    constexpr float camMinY = -(COURSE_HEIGHT - SCREEN_HEIGHT);
    constexpr float camMaxY = 0;
    if (ctx.cameraX < camMinX) ctx.cameraX = camMinX;
    if (ctx.cameraX > camMaxX) ctx.cameraX = camMaxX;
    if (ctx.cameraY < camMinY) ctx.cameraY = camMinY;
    if (ctx.cameraY > camMaxY) ctx.cameraY = camMaxY;
#endif
}

void EntityPlayer::die(GameContext& ctx, bool bounce) {
    if (ctx.isDead) return;
    ctx.isDead = true;
    ctx.state = "DEAD";
    AudioManager::stopBGM();
    if (AudioManager::playingHurryUp) {
        AudioManager::playingHurryUp = false;
        StopMusicStream(AudioManager::hurryUpSe);
    }
    ctx.playerYSpeed = 0.0f;
    ctx.playerXSpeed = 0.0f;
    ctx.deathWaitTimer = 0.0f;
    ctx.deathJumped = false;
    ctx.deathBounce = bounce;
    AudioManager::playSoundEffect("player.death");
}

void EntityPlayer::updateDeath(float dt, GameContext& ctx) {
    if (!ctx.deathJumped) {
        ctx.deathWaitTimer += dt;
        if (ctx.deathWaitTimer >= playerDeathWaitTime) {
            ctx.deathJumped = true;
            if (ctx.deathBounce) ctx.playerYSpeed = -playerDeathJumpSpeed;
        }
    } else {
        ctx.playerYSpeed += playerGravity * dt;
        ctx.playerPos.y += ctx.playerYSpeed * dt;
    }
}

void EntityPlayer::render(const GameContext& ctx) const {
    PlayerTexture2DInfo key = {ctx.currentCourseInfo.style, ctx.currentPlayer, ctx.currentAbility};
    auto it = TextureManager::playerTextures.find(key);
    if (it == TextureManager::playerTextures.end() || it->second.empty()) return;
    auto& vec = it->second;
    Texture2D tex = vec[ctx.isDead ? ctx.playerFrames.at("dead0") : ctx.playerAnimFrame];
    Rectangle src = ctx.playerFacingRight ?
        (Rectangle){0, 0, static_cast<float>(tex.width), static_cast<float>(tex.height)} :
        (Rectangle){static_cast<float>(tex.width), 0, -static_cast<float>(tex.width), static_cast<float>(tex.height)};
    float renderOffset = (ctx.currentPlayer == "TOADETTE" && ctx.playerFacingRight) ? playerPlaitsWidth : 0.0f;
    ScreenPos screen = {ctx.playerPos.x + ctx.cameraX - renderOffset, ctx.playerPos.y + ctx.cameraY};
    Rectangle dest = {screen.x, screen.y,
                      static_cast<float>(tex.width) * key.scale, static_cast<float>(tex.height) * key.scale};
    DrawTexturePro(tex, src, dest, (Vector2){0, 0}, 0.0f, WHITE);
}

void EntityPlayer::renderShadow(const GameContext& ctx) const {
    PlayerTexture2DInfo key = {ctx.currentCourseInfo.style, ctx.currentPlayer, ctx.currentAbility};
    auto it = TextureManager::playerTextures.find(key);
    if (it == TextureManager::playerTextures.end() || it->second.empty()) return;
    auto& vec = it->second;
    Texture2D tex = vec[ctx.isDead ? ctx.playerFrames.at("dead0") : ctx.playerAnimFrame];
    Rectangle src = ctx.playerFacingRight ?
        (Rectangle){0, 0, static_cast<float>(tex.width), static_cast<float>(tex.height)} :
        (Rectangle){static_cast<float>(tex.width), 0, -static_cast<float>(tex.width), static_cast<float>(tex.height)};
    float renderOffset = (ctx.currentPlayer == "TOADETTE" && ctx.playerFacingRight) ? playerPlaitsWidth : 0.0f;
    ScreenPos screen = {ctx.playerPos.x + ctx.cameraX - renderOffset, ctx.playerPos.y + ctx.cameraY};
    Rectangle shadowDest = {screen.x + shadowOffset, screen.y + shadowOffset,
                            static_cast<float>(tex.width) * key.scale, static_cast<float>(tex.height) * key.scale};
    DrawTexturePro(tex, src, shadowDest, (Vector2){0, 0}, 0.0f, Fade(BLACK, 0.5f));
}
