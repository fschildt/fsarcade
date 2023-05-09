#pragma once

#include <math/math.h>
#include <util/Stack.h>
#include <renderer/RenderGroup.h>

struct SDL_Window;

class Renderer {
public:
    enum Api {
        API_OPENGL,
    };
    static Renderer *Select(Api api, SDL_Window *window, uint8_t *memory, size_t memory_size);
    virtual ~Renderer();

public:
    virtual bool Init(SDL_Window *window, uint8_t *memory, size_t memory_size) = 0;
    virtual void Draw(RenderGroup *render_group, int width, int height) = 0;
    virtual void Present() = 0;
};

