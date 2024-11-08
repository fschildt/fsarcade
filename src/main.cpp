#include <basic/defs.hpp>
#include <renderer/RenderGroup.hpp>
#include <renderer/Renderer.hpp>
#include <games/Game.hpp>

#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>
#include <GL/gl.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>


int main(int argc, char **argv)
{
    srand(time(0));
    SDL_Window *window = SDL_CreateWindow("fsarcade",
                                          SDL_WINDOWPOS_CENTERED,
                                          SDL_WINDOWPOS_CENTERED,
                                          400, 800,
                                          SDL_WINDOW_OPENGL);
    if (!window) {
        return 0;
    }

    SDL_GLContext sdl_gl_context = SDL_GL_CreateContext(window);
    if (!sdl_gl_context) {
        return 0;
    }

    Renderer *renderer = Renderer::Select(Renderer::API_OPENGL, window);
    RenderGroup render_group;
    Game *game = Game::Select(Game::TETRIS);

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

