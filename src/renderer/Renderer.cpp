#include <renderer/Renderer.hpp>
#include <renderer/gl_renderer/GlRenderer.hpp>


std::unique_ptr<Renderer>
Renderer::Select(Api api, SDL_Window *window) {
    switch (api) {
        case API_OPENGL: {
            return std::make_unique<GlRenderer>(window);
        }
        InvalidDefaultCase;
    }

    return nullptr;
}


Renderer::~Renderer() {}
