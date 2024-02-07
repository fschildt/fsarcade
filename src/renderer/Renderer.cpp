#include <renderer/Renderer.hpp>
#include <renderer/gl_renderer/GlRenderer.hpp>

#include <new>

static Renderer *s_SelectedRenderer;

Renderer::~Renderer() {
}

Renderer *Renderer::Select(Api api, SDL_Window *window, uint8_t *memory, size_t memory_size) {
    Renderer *renderer = new (memory) GlRenderer();
    if (!renderer->Init(window, memory + sizeof(GlRenderer), memory_size - sizeof(GlRenderer))) {
        return 0;
    }
    return renderer;
}

