#include <renderer/Renderer.h>
#include <renderer/gl_renderer/GlRenderer.h>

static Renderer *s_SelectedRenderer;

Renderer::~Renderer() {
}

Renderer *Renderer::Select(Api api, SDL_Window *window, Stack *stack) {
    if (s_SelectedRenderer) {
        delete s_SelectedRenderer;
    }

    Renderer *renderer = new GlRenderer();
    if (!renderer->Init(window, stack)) {
        delete renderer;
    }

    s_SelectedRenderer = renderer;
    return renderer;
}

