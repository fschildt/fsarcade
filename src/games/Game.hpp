#pragma once

#include <basic/defs.hpp>
#include <SDL3/SDL.h>
#include <memory>
#include <vector>
#include <renderer/RenderGroup.hpp>

struct SDL_Window;

class Game {
public:
    enum GameType {
        NO_GAME,
        TETRIS,
        SNAKE
    };

    Game() = default;
    virtual ~Game();
    static std::unique_ptr<Game> Select(GameType type);

    virtual void Init() = 0;
    virtual bool Update(std::vector<SDL_Event> &events, RenderGroup &render_group) = 0;
};

