#include <basic/defs.hpp>
#include <cstdlib>
#include <memory>
#include <renderer/RenderGroup.hpp>
#include <renderer/Renderer.hpp>
#include <games/Game.hpp>

#include <SDL3/SDL.h>
#include <SDL3/SDL_video.h>
#include <GL/gl.h>

#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_sdl3.h>

#include <stdlib.h>
#include <assert.h>
#include <iostream>
#include <array>

int
main(int argc, char **argv)
{
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD)) {
        std::cerr << "Failed to init SDL3: " << SDL_GetError() << '\n';
        return EXIT_FAILURE;
    }

    const char* glsl_version = "#version 130";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

    SDL_Window *window = SDL_CreateWindow("fsarcade", 400, 800, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN | SDL_WINDOW_HIGH_PIXEL_DENSITY);
    if (!window) {
        std::cerr << "Failed to create SDL_window: " << SDL_GetError() << '\n';
        return EXIT_FAILURE;
    }

    SDL_GLContext sdl_gl_context = SDL_GL_CreateContext(window);
    if (!sdl_gl_context) {
        std::cerr << "Failed to create SDL_GLContext: " << SDL_GetError() << '\n';
        SDL_DestroyWindow(window);
        return EXIT_FAILURE;
    }

    SDL_GL_MakeCurrent(window, sdl_gl_context);
    SDL_GL_SetSwapInterval(1); // Enable vsync
    SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
    SDL_ShowWindow(window);


    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplSDL3_InitForOpenGL(window, sdl_gl_context);
    ImGui_ImplOpenGL3_Init(glsl_version);


    std::unique_ptr<Game> game = Game::Select(Game::TETRIS);
    game->Init();

    std::unique_ptr<Renderer> renderer = Renderer::Select(Renderer::API_OPENGL, window);
    if (!renderer->Init()) {
        return EXIT_FAILURE;
    }

    RenderGroup render_group;


    std::vector<SDL_Event> game_events;
    game_events.reserve(32);
    for (;;) {
        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();


        size_t cur_game_events = 0;
        size_t max_game_events = game_events.max_size();

        SDL_Event event;
        while (cur_game_events < max_game_events && SDL_PollEvent(&event)) {
            if (io.WantCaptureKeyboard || io.WantCaptureMouse) {
                ImGui_ImplSDL3_ProcessEvent(&event);
            }
            if (event.type == SDL_EVENT_QUIT)
                goto QUIT;
            if (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED && event.window.windowID == SDL_GetWindowID(window))
                goto QUIT;
            // Todo: can i still get the window id?
            if (event.type == SDL_EVENT_WINDOW_DESTROYED && event.window.windowID == SDL_GetWindowID(window)) {
                goto QUIT;
            }

            game_events.emplace_back(event);
        }

        if (!game->Update(game_events, render_group)) {
            goto QUIT;
        }
        game_events.clear();



        // Rendering
        ImGui::Render();
        ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
        glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);

        int w, h;
        SDL_GetWindowSize(window, &w, &h);
        renderer->Draw(render_group, w, h);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        renderer->Present();
        render_group.Reset();

    }

QUIT:

    return 0;
}

