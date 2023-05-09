#pragma once

#include <games/Game.h>
#include <games/tetris/PlayerState.h>

class Tetromino;

class Tetris : public Game {
public:
    Tetris(void *memory, size_t memory_size);
    void Init() override;
    void Update(const GameInput *input, RenderGroup *renderer_group) override;
private:
    PlayerState m_PlayerState;
};

