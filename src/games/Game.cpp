#include <basic/defs.hpp>
#include <games/Game.hpp>
#include <games/tetris/Tetris.hpp>
#include <games/snake/Snake.hpp>
#include <games/minesweeper/Minesweeper.hpp>

#include <assert.h>
#include <memory>


std::unique_ptr<Game>
Game::Select(GameType type)
{
    switch (type) {
        case NO_GAME: {
            return nullptr;
        }

        case TETRIS: {
            return std::make_unique<Tetris>();
        } break;

        case SNAKE: {
            return std::make_unique<Snake>();
        } break;

        case MINESWEEPER: {
            return std::make_unique<Minesweeper>();
        } break;

        InvalidDefaultCase;
    }

    return nullptr;
}


Game::~Game() {}

