#include "basic/defs.hpp"
#include <games/Game.hpp>
#include <games/tetris/Tetris.hpp>

#include <assert.h>
#include <memory>


std::unique_ptr<Game>
Game::Select(GameType type)
{
    switch (type) {
        case TETRIS: {
            return std::make_unique<Tetris>();
        } break;

        InvalidDefaultCase;
    }

    return 0;
}


Game::~Game() {}

