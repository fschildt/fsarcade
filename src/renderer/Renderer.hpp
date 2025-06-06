#pragma once

#include <basic/math.hpp>
#include <renderer/RenderGroup.hpp>
#include <SDL3/SDL.h>
#include <SDL3/SDL_video.h>

#include <memory>

class Renderer {
public:
    enum Api {
        API_OPENGL,
    };

    Renderer() = default;
    static std::unique_ptr<Renderer> Select(Api api, SDL_Window *window);


public:
    virtual ~Renderer() = 0;
    virtual bool Init() = 0;
    virtual void Draw(RenderGroup &render_group, int width, int height) = 0;
    virtual void Present() = 0;
};

