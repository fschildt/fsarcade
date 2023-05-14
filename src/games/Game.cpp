#include <games/Game.h>
#include <games/tetris/Tetris.h>
#include <generated/asset_font.h>

#include <new>
#include <assert.h>

Game* Game::Select(GameType type, uint8_t *memory, size_t memory_size) {
    Game *game = 0;
    void *assets_memory = 0;

    switch (type) {
        case TETRIS: {
            assets_memory = g_asset_font;
            game = new (memory) Tetris();
            game->Init(memory + sizeof(Tetris), memory_size - sizeof(Tetris), 0);
        } break;

        default: assert(0);
    }

    return game;
}

