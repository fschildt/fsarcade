#pragma once

#include <basic/math.hpp>
#include <renderer/RenderGroup.hpp>
#include <SDL3/SDL.h>
#include <SDL3/SDL_video.h>

class Renderer {
public:
    enum Api {
        API_OPENGL,
    };
    static Renderer *Select(Api api, SDL_Window *window);
    virtual ~Renderer();

public:
    virtual bool Init(SDL_Window *window) = 0;
    virtual void Draw(RenderGroup& render_group, int width, int height) = 0;
    virtual void Present() = 0;
};

