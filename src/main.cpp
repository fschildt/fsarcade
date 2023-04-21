#include <cstdint>
#include <games/Game.h>
#include <renderer/Renderer.h>

#include <SDL2/SDL_events.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <GL/gl.h>

#define MEBIBYTES(x) (1024*1024*x)

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
                this->controller1.MoveRight = true;
            } else if (keycode == SDLK_LEFT) {
                this->controller1.MoveLeft = true;
            } else if (keycode == SDLK_DOWN) {
                this->controller1.MoveDown = true;
            } else if (keycode == SDLK_z) {
                this->controller1.ActionA = true;
            } else if (keycode == SDLK_x) {
                this->controller1.ActionB = true;
            }
        }
    }
}

int main(int argc, char **argv)
{
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


    size_t memory_size = MEBIBYTES(32);
    uint8_t *memory = (uint8_t*)malloc(memory_size);
    Stack stack;
    stack.Init(memory, memory_size);

    Renderer *renderer = Renderer::Select(Renderer::API_OPENGL, sdl_window, &stack);
    RenderGroup render_group(&stack);

    Game *game = Game::Select(Game::TETRIS);
    game->Init();


    for (;;) {
        GameInput game_input = {};
        game_input.Gather(sdl_window);

        static int counter = 0;
//        printf("%d\n", counter++);

        if (game_input.quit) {
            break;
        }

        game->Update(&game_input, &render_group);

        int w, h;
        SDL_GetWindowSize(sdl_window, &w, &h);
        renderer->Draw(&render_group, w, h);
        renderer->Present();
        render_group.Reset();
    }
}

