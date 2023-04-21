#pragma once

#include <games/Game.h>
#include <games/tetris/PlayerState.h>

#define ARRAY_COUNT(array) (sizeof(array) / sizeof(array[0]))

class Tetromino;

class Tetris : public Game {
public:
    void Init() override;
    void Update(const GameInput *input, RenderGroup *renderer_group) override;
private:
    int32_t m_PlayerCount;
    PlayerState m_PlayerStates[2];
};

