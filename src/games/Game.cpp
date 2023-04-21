#include <games/Game.h>
#include <games/tetris/Tetris.h>

#include <assert.h>

Game* Game::Select(GameType type) {
    Game *game;
    switch (type) {
        case TETRIS: {
            game = new Tetris();
        } break;

        default: assert(0);
    }

    return game;
}

