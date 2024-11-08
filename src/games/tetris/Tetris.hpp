#pragma once

#include <games/Game.hpp>
#include <games/tetris/Tetromino.hpp>
#include <games/tetris/Board.hpp>

#include <renderer/RenderGroup.hpp>


class Tetris : public Game {
public:
    void Init() override;
    bool Update(SDL_Window *window, RenderGroup& render_group) override;
    void HandleTetrominoPlacement();

private:
    int32_t GetDropCount(float dt);

private:
    int32_t m_Level;
    float m_DtRemaining;
    uint64_t m_TLast;
    Tetromino m_ActiveTetromino;
    Tetromino m_NextTetromino;
    Board m_Board;
};

