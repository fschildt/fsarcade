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
#include <stdio.h>
#include <assert.h>

void GameInput::Gather(SDL_Window *window) {
    this->quit = false;

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            this->quit = true;
        }
        else if (event.type == SDL_WINDOWEVENT) {
            SDL_WindowEvent *window_event = &event.window;
            if (window_event->event == SDL_WINDOWEVENT_CLOSE) {
                this->quit = true;
            }
        }
        else if (event.type == SDL_TEXTINPUT) {
            char *text = event.text.text;
        }
        else if (event.type == SDL_KEYDOWN) {
            auto keycode = event.key.keysym.sym;
            if (keycode == SDLK_RIGHT) {
                this->controller.MoveRight = true;
            } else if (keycode == SDLK_LEFT) {
                this->controller.MoveLeft = true;
            } else if (keycode == SDLK_DOWN) {
                this->controller.MoveDown = true;
            } else if (keycode == SDLK_x) {
                this->controller.ActionA = true;
            } else if (keycode == SDLK_z) {
                this->controller.ActionB = true;
            }
        }
    }

    static uint64_t tlast = SDL_GetTicks64();
    uint64_t tnow = SDL_GetTicks64();
    this->dt = (tnow - tlast) / 1000.0f; // milliseconds to seconds
    tlast = tnow;
}

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
        GameInput game_input = {};
        game_input.Gather(window);

        if (game_input.quit) {
            break;
        }

        game->Update(game_input, render_group);

        int w, h;
        SDL_GetWindowSize(window, &w, &h);
        renderer->Draw(render_group, w, h);
        renderer->Present();
        render_group.Reset();
    }
}

