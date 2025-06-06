#include <basic/defs.hpp>
#include <memory>
#include <renderer/RenderGroup.hpp>
#include <renderer/Renderer.hpp>
#include <games/Game.hpp>

#include <SDL3/SDL.h>
#include <SDL3/SDL_video.h>
#include <GL/gl.h>

#include <stdlib.h>
#include <assert.h>
#include <iostream>

int
main(int argc, char **argv)
{
    SDL_Window *window = SDL_CreateWindow("fsarcade", 400, 800, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    if (!window) {
        std::cerr << "Failed to create SDL_window: " << SDL_GetError() << '\n';
        return 0;
    }

    SDL_GLContext sdl_gl_context = SDL_GL_CreateContext(window);
    if (!sdl_gl_context) {
        std::cerr << "Failed to create SDL_GLContext: " << SDL_GetError() << '\n';
        SDL_DestroyWindow(window);
        return 0;
    }


    std::unique_ptr<Game> game = Game::Select(Game::TETRIS);
    game->Init();

    std::unique_ptr<Renderer> renderer = Renderer::Select(Renderer::API_OPENGL, window);
    if (!renderer->Init()) {
        return 0;
    }

    RenderGroup render_group;


    for (;;) {
        if (!game->Update(window, render_group)) {
            break;
        }

        int w, h;
        SDL_GetWindowSize(window, &w, &h);
        renderer->Draw(render_group, w, h);
        renderer->Present();
        render_group.Reset();
    }
}

