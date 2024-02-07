#pragma once

#include <games/Game.hpp>
#include <games/tetris/Tetromino.hpp>
#include <games/tetris/Board.hpp>

#include <renderer/Renderer.hpp>
#include <util/MemArena.hpp>


class Tetris : public Game {
public:
    void Init(uint8_t *memory, size_t memory_size) override;
    void Update(GameInput *input, RenderGroup *render_group) override;

private:
    int32_t GetHarddropCount(float dt);

private:
    int32_t m_Level;
    float m_DtRemained;
    Tetromino m_NextTetromino;
    Board m_Board;
};

