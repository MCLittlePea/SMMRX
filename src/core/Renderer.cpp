#include "Renderer.hpp"
#include <algorithm>

void Renderer::init() {
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "SMMRX - Super Mario Maker RX");
    SetExitKey(KEY_NULL);
    SetWindowState(FLAG_WINDOW_RESIZABLE);
    SetWindowMinSize(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);
    SetTargetFPS(60);
    gameRenderTarget = LoadRenderTexture(SCREEN_WIDTH, SCREEN_HEIGHT);
}

void Renderer::beginFrame() {
    BeginTextureMode(gameRenderTarget);
}

void Renderer::endFrame() {
    EndTextureMode();
    BeginDrawing();
    ClearBackground(BLACK);
    float winW = static_cast<float>(GetScreenWidth());
    float winH = static_cast<float>(GetScreenHeight());
    letterboxScale = std::min(winW / SCREEN_WIDTH, winH / SCREEN_HEIGHT);
    float drawW = SCREEN_WIDTH * letterboxScale;
    float drawH = SCREEN_HEIGHT * letterboxScale;
    letterboxX = (winW - drawW) / 2.0f;
    letterboxY = (winH - drawH) / 2.0f;
    Rectangle src = {0.0f, static_cast<float>(gameRenderTarget.texture.height),
                     static_cast<float>(gameRenderTarget.texture.width),
                     -static_cast<float>(gameRenderTarget.texture.height)};
    Rectangle dest = {letterboxX, letterboxY, drawW, drawH};
    DrawTexturePro(gameRenderTarget.texture, src, dest, (Vector2){0, 0}, 0.0f, WHITE);
    EndDrawing();
}

void Renderer::close() {
    UnloadRenderTexture(gameRenderTarget);
    CloseWindow();
}

Vector2 Renderer::screenToGamePos(Vector2 screenPos) {
    return {
        (screenPos.x - letterboxX) / letterboxScale,
        (screenPos.y - letterboxY) / letterboxScale
    };
}

Vector2 Renderer::getGameMousePosition() {
    return screenToGamePos(GetMousePosition());
}
