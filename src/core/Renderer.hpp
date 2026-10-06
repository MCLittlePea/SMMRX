#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <raylib.h>
#include "../settings/window_settings.hpp"
#include "../settings/render_settings.hpp"

class Renderer {
public:
    RenderTexture2D gameRenderTarget;
    float letterboxScale = 1.0f;
    float letterboxX = 0.0f;
    float letterboxY = 0.0f;

    void init();
    void beginFrame();
    void endFrame();
    void close();
    Vector2 screenToGamePos(Vector2 screenPos);
    Vector2 getGameMousePosition();
};

#endif
