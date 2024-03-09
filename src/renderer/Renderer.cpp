#include <renderer/Renderer.hpp>
#include <renderer/gl_renderer/GlRenderer.hpp>

#include <new>

static Renderer *s_SelectedRenderer;

Renderer::~Renderer() {
}

Renderer *Renderer::Select(Api api, SDL_Window *window) {
    switch (api) {
        case API_OPENGL: {
            Renderer *renderer = new GlRenderer();
            if (!renderer->Init(window)) {
                return 0;
            }
            return renderer;
        }
        InvalidDefaultCase;
    }
    return 0;
}

