#include <games/Game.hpp>
#include <games/tetris/Tetris.hpp>

#include <new>
#include <assert.h>

Game* Game::Select(GameType type, uint8_t *memory, size_t memory_size) {
    Game *game = 0;

    switch (type) {
        case TETRIS: {
            game = new (memory) Tetris();
            game->Init(memory + sizeof(Tetris), memory_size - sizeof(Tetris));
        } break;

        default: assert(0);
    }

    return game;
}
