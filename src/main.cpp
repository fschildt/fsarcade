#include "imgui_internal.h"
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


Game::GameType
do_menu(RenderGroup &render_group)
{
    Game::GameType type = Game::NO_GAME;

    ImGuiIO& io = ImGui::GetIO();
    //ImGuiWindowFlags flags = ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
    ImGuiWindowFlags flags = 0;

    ImGui::Begin("Game Selection", nullptr, flags);
    if (ImGui::Button("Tetris")) {
        type = Game::TETRIS;
    }
    if (ImGui::Button("Snake")) {
        type = Game::SNAKE;
    }
    if (ImGui::Button("Minesweeper")) {
        type = Game::MINESWEEPER;
    }
    ImGui::End();


    V3F32 clear_color = V3F32(0.4f, 0.4f, 0.4f);
    render_group.SetCameraSize(4.0f, 3.0f);
    render_group.Clear(clear_color);


    return type;
}


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

    SDL_Window *window = SDL_CreateWindow("fsarcade", 1024, 768, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN | SDL_WINDOW_HIGH_PIXEL_DENSITY);
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
    SDL_GL_SetSwapInterval(1); // enable vsync
    SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
    SDL_ShowWindow(window);


    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

    ImGui::StyleColorsDark();
    ImGui_ImplSDL3_InitForOpenGL(window, sdl_gl_context);
    ImGui_ImplOpenGL3_Init(glsl_version);


    std::unique_ptr<Game> game = nullptr;
    std::unique_ptr<Renderer> renderer = Renderer::Select(Renderer::API_OPENGL, window);
    if (!renderer->Init()) {
        return EXIT_FAILURE;
    }

    RenderGroup render_group;
    SDL_GetWindowSize(window, &render_group.m_ScreenWidth, &render_group.m_ScreenHeight);


    std::vector<SDL_Event> game_events;
    game_events.reserve(32);
    for (;;) {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();


        size_t cur_game_events = 0;
        size_t max_game_events = game_events.max_size();

        SDL_Event event;
        while (cur_game_events < max_game_events && SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_KEY_DOWN ||
                event.type == SDL_EVENT_MOUSE_BUTTON_DOWN)
            {
                game_events.emplace_back(event);
                cur_game_events++;
            }
            ImGui_ImplSDL3_ProcessEvent(&event);

            if (event.type == SDL_EVENT_QUIT)
                goto QUIT;
            if (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED && event.window.windowID == SDL_GetWindowID(window))
                goto QUIT;
            if (event.type == SDL_EVENT_WINDOW_DESTROYED && event.window.windowID == SDL_GetWindowID(window)) {
                goto QUIT;
            }
        }


        int w, h;
        SDL_GetWindowSize(window, &w, &h);
        render_group.m_ScreenWidth = w;
        render_group.m_ScreenHeight = h;


        if (game) {
            bool keep_game_running = game->Update(game_events, render_group);
            if (!keep_game_running) {
                game.reset();
            }
        }
        else {
            Game::GameType type = do_menu(render_group);
            if (type != Game::NO_GAME) {
                game = Game::Select(type);
            }
        }
        game_events.clear();



        ImGuiIO& io = ImGui::GetIO();
        ImGuiWindowFlags flags = ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;

        ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x - 200, 0));
        ImGui::Begin("Performance", nullptr, flags);
        ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
        ImGui::End();


        renderer->Draw(render_group);
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        renderer->Present();
        render_group.Reset();
    }

QUIT:
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();

    SDL_GL_DestroyContext(sdl_gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}

