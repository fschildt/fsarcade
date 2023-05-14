#pragma once

#include <games/Game.h>
#include <games/tetris/PlayerState.h>

class Tetromino;

class Tetris : public Game {
public:
    Tetris() = default;
    void Init(uint8_t *memory, size_t memory_size, GameFont *font) override;
    void Update(const GameInput *input, RenderGroup *renderer_group) override;
private:
    PlayerState m_PlayerState;
};

