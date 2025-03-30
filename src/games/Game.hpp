#pragma once

#include <basic/defs.hpp>
#include <SDL3/SDL.h>

class RenderGroup;
struct SDL_Window;

class Game {
public:
    enum GameType {
        NO_GAME,
        TETRIS,
    };
    static Game *Select(GameType type);

public:
    virtual void Init() = 0;
    virtual bool Update(SDL_Window *window, RenderGroup& render_group) = 0;
};

