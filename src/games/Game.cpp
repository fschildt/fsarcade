#include <games/Game.hpp>
#include <games/tetris/Tetris.hpp>

#include <assert.h>

Game* Game::Select(GameType type)
{
    Game *game = 0;

    switch (type) {
        case TETRIS: {
            game = new Tetris();
            game->Init();
        } break;

        default: assert(0);
    }

    return game;
}

