#include <defs.hpp>
#include <util/MemArena.hpp>
#include <renderer/RenderGroup.hpp>
#include <renderer/Renderer.hpp>
#include <games/Game.hpp>

#include <new>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <GL/gl.h>
#include <time.h>

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
    SDL_Window *sdl_window = SDL_CreateWindow("fsarcade",
                                          SDL_WINDOWPOS_CENTERED,
                                          SDL_WINDOWPOS_CENTERED,
                                          400, 800,
                                          SDL_WINDOW_OPENGL);
    if (!sdl_window) {
        return 0;
    }

    SDL_GLContext sdl_gl_context = SDL_GL_CreateContext(sdl_window);
    if (!sdl_gl_context) {
        return 0;
    }


    // Todo: just use new/delete

    size_t memory_size = MEBIBYTES(6);
    uint8_t *memory = (uint8_t*)malloc(memory_size);
    MemArena arena(memory, memory_size);

    size_t render_group_memory_size = MEBIBYTES(2);
    uint8_t *render_group_memory = (uint8_t*)arena.Push(render_group_memory_size);
    RenderGroup *render_group = new (render_group_memory) RenderGroup(render_group_memory + sizeof(RenderGroup), render_group_memory_size - sizeof(RenderGroup));

    size_t renderer_memory_size = MEBIBYTES(2);
    uint8_t *renderer_memory = (uint8_t*)arena.Push(renderer_memory_size);
    Renderer *renderer = Renderer::Select(Renderer::API_OPENGL, sdl_window, renderer_memory, renderer_memory_size);

    size_t game_memory_size = MEBIBYTES(2);
    uint8_t *game_memory = (uint8_t*)arena.Push(game_memory_size);
    Game *game = Game::Select(Game::TETRIS, game_memory, game_memory_size);


    for (;;) {
        GameInput game_input = {};
        game_input.Gather(sdl_window);

        if (game_input.quit) {
            break;
        }

        game->Update(&game_input, render_group);

        int w, h;
        SDL_GetWindowSize(sdl_window, &w, &h);
        renderer->Draw(render_group, w, h);
        renderer->Present();
        render_group->Reset();
    }
}

