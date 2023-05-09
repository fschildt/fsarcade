#include <renderer/Renderer.h>
#include <renderer/gl_renderer/GlRenderer.h>

#include <new>

static Renderer *s_SelectedRenderer;

Renderer::~Renderer() {
}

Renderer *Renderer::Select(Api api, SDL_Window *window, uint8_t *memory, size_t memory_size) {
    // Todo: "deconstruct" old renderer?

    Renderer *renderer = new (memory) GlRenderer();
    if (!renderer->Init(window, memory + sizeof(GlRenderer), memory_size - sizeof(GlRenderer))) {
        return 0;
    }
    return renderer;
}

