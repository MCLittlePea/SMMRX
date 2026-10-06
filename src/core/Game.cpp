#include "Game.hpp"
#include "../render/TextureManager.hpp"
#include "../audio/AudioManager.hpp"
#include "../ui/LanguageManager.hpp"
#include "../settings/gui_settings.hpp"
#include "../settings/render_settings.hpp"
#include "../settings/course_settings.hpp"
#include "../blocks/BlockRegistry.hpp"
#include "../registry/Registry.hpp"
#include "../registry/Meta.hpp"
#include <cmath>
#include <algorithm>

using namespace std;

void registerAll();

void Game::initLevel() {
    auto addBlock = [this](BlockPos pos, const string& id) {
        ctx.levelBlocks.push_back({pos, id});
    };
    for (int i = 1; i <= 10; i++) addBlock({i, 28},"ground");
    for (int i = 1; i <= 10; i++) addBlock({i, 27},"ground");
    for (int i = 12; i <= 200; i++) addBlock({i, 28},"ground");
    for (int i = 12; i <= 200; i++) addBlock({i, 27},"ground");
    addBlock({5,25},"ground");
    addBlock({5,24},"ground");
    addBlock({5,23},"ground");
    addBlock({5,22},"ground");
    addBlock({5,21},"ground");
    addBlock({2,24},"hard_block");
    addBlock({3,24},"hard_block");
    addBlock({7,24},"hard_block");
    addBlock({8,24},"hard_block");
    addBlock({15,26},"hard_block");
    addBlock({15,25},"hard_block");
    addBlock({15,24},"hard_block");
    addBlock({15,23},"hard_block");
    addBlock({17,26},"hard_block");
    addBlock({17,25},"hard_block");
    addBlock({17,24},"hard_block");
    addBlock({17,23},"hard_block");
    addBlock({18,23},"hard_block");
    addBlock({19,23},"hard_block");
    addBlock({20,23},"hard_block");
    addBlock({21,23},"hard_block");
    addBlock({22,23},"hard_block");
    addBlock({30,24},"spike_trap");
    addBlock({32,24},"spike_trap");
}

void Game::updateBackgroundAnim(float dt) {
    auto it = TextureManager::backgrounds.find(ctx.currentCourseInfo);
    if (it == TextureManager::backgrounds.end() || it->second.size() <= 1) return;
    ctx.bgAnimTimer += dt;
    if (ctx.bgAnimTimer >= bgAnimFrameDuration) {
        ctx.bgAnimTimer = 0.0f;
        ctx.bgAnimFrame = (ctx.bgAnimFrame + 1) % static_cast<int>(it->second.size());
    }
}

void Game::renderBackground() {
    auto it = TextureManager::backgrounds.find(ctx.currentCourseInfo);
    if (it == TextureManager::backgrounds.end() || it->second.empty()) return;
    Texture2D tex = it->second[ctx.bgAnimFrame % static_cast<int>(it->second.size())];
    for (int i = 0; i * bgSize + bgOffsetX < COURSE_WIDTH; i++) {
        float px = floorf(i * bgSize + bgOffsetX + ctx.cameraX);
        float py = floorf(bgY + ctx.cameraY);
        DrawTextureEx(tex, (Vector2){px, py}, 0.0f, static_cast<float>(SCALE), WHITE);
    }
}

void Game::init() {
    registerAll();
    renderer.init();
    LanguageManager::loadLanguages();
    LanguageManager::initFont();
    AudioManager::init();
    AudioManager::loadSoundEffectsFromCSV("assets/data/ses.csv");

    TextureManager::loadBlockFromField({{144, 112}}, "ground");
    TextureManager::loadBlockFromField({{16, 0}}, "block");
    TextureManager::loadBlockFromField({{96, 0}}, "hard_block");
    TextureManager::loadBlockFromField({{32, 64}}, "spike_trap");
    TextureManager::loadBlocksFromCSV("assets/data/blocks.csv");
    TextureManager::loadBackgroundsFromCSV("assets/data/backgrounds.csv");
    TextureManager::loadPlayersFromCSV("assets/data/players.csv");
    AudioManager::loadBGMLoopsFromCSV("assets/data/bgm_loop.csv");
    AudioManager::loadBGMFromTheme("SMB1", "Ground");
    AudioManager::loadBGMFromTheme("SMB1", "Underground");
    AudioManager::loadBGMFromTheme("SMB1", "Underwater");
    AudioManager::hurryUpSe = LoadMusicStream("assets/musics/SMB1/Hurry.mp3");
    TextureManager::loadGuiFromCSV("assets/data/guis.csv");

    hud.init(ctx);
    initLevel();

#if !ENABLE_ANIMATION
    ctx.state = "ANIMATION";
#endif
}

void Game::updateStart(float dt) {
    ctx.startTimer += dt;
    if (ctx.startTimer >= 2.0f) ctx.startClickable = true;
    if (ctx.startClickable && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        Vector2 gp = renderer.getGameMousePosition();
        if (gp.x >= 0 && gp.x <= SCREEN_WIDTH && gp.y >= 0 && gp.y <= SCREEN_HEIGHT) {
            ctx.state = "ANIMATION";
        }
    }
}

void Game::updateAnimation(float dt) {
    ctx.animTimer += dt;
    if (ctx.animTimer >= animDuration) {
        ctx.state = "GAME";
        AudioManager::musicLogicalPos = 0.0f;
        ctx.gameOver = false;
        ctx.isDead = false;
        AudioManager::hurryUpTriggered = false;
        if (ctx.currentTime <= 100) {
            AudioManager::hurryUpTriggered = true;
            PlayMusicStream(AudioManager::hurryUpSe);
            SetMusicVolume(AudioManager::hurryUpSe, AudioManager::bgmVolume * 0.5f);
            AudioManager::playingHurryUp = true;
            AudioManager::hurryUpTimer = 0.0f;
            AudioManager::hurryUpPendingStyle = "SMB1";
            AudioManager::hurryUpPendingTheme = ctx.currentCourseInfo.theme;
            AudioManager::hurryUpPendingType = (ctx.currentCourseInfo.time == "Night") ? "PlayMoonHurry" : "PlayHurry";
        } else {
            string bgmType = (ctx.currentCourseInfo.time == "Night") ? "PlayMoon" : "PlayNormal";
            AudioManager::playBGM("SMB1", ctx.currentCourseInfo.theme, bgmType);
        }
        hud.start();
    }
}

void Game::updateGame(float dt) {
    int timeBefore = static_cast<int>(ctx.currentTime);
    if (!ctx.isDead) hud.update(dt, ctx);
    int timeAfter = static_cast<int>(ctx.currentTime);
    if (!ctx.isDead && ctx.currentTime <= 0.0f) {
        player.die(ctx);
    }
    if (!AudioManager::hurryUpTriggered && timeBefore > 99 && timeAfter <= 99) {
        AudioManager::hurryUpTriggered = true;
        AudioManager::stopBGM();
        PlayMusicStream(AudioManager::hurryUpSe);
        SetMusicVolume(AudioManager::hurryUpSe, AudioManager::bgmVolume * 0.5f);
        AudioManager::playingHurryUp = true;
        AudioManager::hurryUpTimer = 0.0f;
        AudioManager::hurryUpPendingStyle = "SMB1";
        AudioManager::hurryUpPendingTheme = ctx.currentCourseInfo.theme;
        AudioManager::hurryUpPendingType = (ctx.currentCourseInfo.time == "Night") ? "PlayMoonHurry" : "PlayHurry";
    }
    player.update(dt, ctx);
}

void Game::renderStart() {
    ClearBackground(RAYWHITE);
    const char* title = LanguageManager::langTextC("gui.title");
    const char* sub = LanguageManager::langTextC("gui.subtitle");
    int tSize = static_cast<int>(startTitleSize * guiScale);
    int sSize = static_cast<int>(startSubtitleSize * guiScale);
    int tw = LanguageManager::measureText(title, tSize);
    int sw = LanguageManager::measureText(sub, sSize);
    LanguageManager::drawText(title, (SCREEN_WIDTH - tw) / 2, static_cast<int>(SCREEN_HEIGHT / 2 - startTitleOffsetY * guiScale), tSize, DARKGRAY);
    LanguageManager::drawText(sub, (SCREEN_WIDTH - sw) / 2, static_cast<int>(SCREEN_HEIGHT / 2 + startSubtitleOffsetY * guiScale), sSize, GRAY);
    if (ctx.startClickable) {
        const char* hint = LanguageManager::langTextC("gui.click_to_start");
        int hintSize = static_cast<int>(startHintSize * guiScale);
        int hw = LanguageManager::measureText(hint, hintSize);
        LanguageManager::drawText(hint, (SCREEN_WIDTH - hw) / 2, static_cast<int>(SCREEN_HEIGHT - startHintOffsetBottom * guiScale), hintSize, LIGHTGRAY);
    }
}

void Game::renderAnimation() {
    ClearBackground(BLACK);
    float a = ctx.animTimer < 1.0f ? ctx.animTimer :
              (ctx.animTimer > animDuration - 1.0f ? animDuration - ctx.animTimer : 1.0f);
    const char* animText = LanguageManager::langTextC("gui.intro_animation");
    int animSize = static_cast<int>(animTextSize * guiScale);
    LanguageManager::drawText(animText, (SCREEN_WIDTH - LanguageManager::measureText(animText, animSize)) / 2,
        static_cast<int>(SCREEN_HEIGHT / 2 - animTextOffsetY * guiScale), animSize, Fade(WHITE, a));
}

void Game::renderGame() {
    ClearBackground(SKYBLUE);
    renderBackground();
    for (int i = 0; i < static_cast<int>(ctx.levelBlocks.size()); i++) {
        BlockBase* b = BlockRegistry::create(ctx.levelBlocks[i].id, ctx.levelBlocks[i].pos);
        if (b) { b->renderShadow(ctx); delete b; }
    }
    player.renderShadow(ctx);
    for (int i = 0; i < static_cast<int>(ctx.levelBlocks.size()); i++) {
        BlockBase* b = BlockRegistry::create(ctx.levelBlocks[i].id, ctx.levelBlocks[i].pos);
        if (b) { b->render(ctx); delete b; }
    }
    player.render(ctx);
    if (ctx.debugMode) {
        LanguageManager::drawText(TextFormat(LanguageManager::langTextC("debug.speed"), fabs(ctx.playerXSpeed) / BLOCK_PX),
            static_cast<int>(debugTextOffsetX * guiScale), static_cast<int>(debugTextOffsetY * guiScale),
            static_cast<int>(debugTextSize * guiScale), WHITE);
    }
    hud.draw(ctx);
}

void Game::run() {
    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        if (dt > 0.05f) dt = 0.05f;

        AudioManager::updateStreams(dt);
        AudioManager::updateBGMFade(dt);
        updateBackgroundAnim(dt);

        if (ctx.state == "START") updateStart(dt);
        else if (ctx.state == "ANIMATION") updateAnimation(dt);
        else if (ctx.state == "GAME") updateGame(dt);
        else if (ctx.state == "DEAD") player.updateDeath(dt, ctx);

        renderer.beginFrame();
        if (ctx.state == "START") renderStart();
        else if (ctx.state == "ANIMATION") renderAnimation();
        else if (ctx.state == "DEAD" || ctx.state == "GAME") renderGame();
        renderer.endFrame();
    }
}

void Game::close() {
    TextureManager::unloadAll();
    AudioManager::unloadAll();
    hud.unload();
    AudioManager::close();
    renderer.close();
}
