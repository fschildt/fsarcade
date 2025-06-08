#pragma once

#include <games/Game.hpp>
#include <games/tetris/Tetromino.hpp>
#include <games/tetris/Board.hpp>
#include <renderer/RenderGroup.hpp>


class Tetris : public Game {
public:
    Tetris();
    void Init() override;
    bool Update(std::vector<SDL_Event> &events, RenderGroup& render_group) override;
    void HandleTetrominoPlacement();

private:
    void UpdateRunning(SDL_Event &event, float dt);
    void UpdatePaused(SDL_Event &event);

    int32_t GetDropCount(float dt);
    void DoImGui();

private:
    float m_DtRemaining = 0.f;
    uint64_t m_TLast = SDL_GetTicks();

    Tetromino m_ActiveTetromino;
    Tetromino m_NextTetromino;
    Board m_Board;

    bool m_Paused = false;
    bool m_Running = true;

    uint32_t m_TetrominoCounters[TETROMINO_ID_COUNT] = {};
    uint32_t m_Score = 0;
    uint32_t m_LineCounter = 0;
    uint32_t m_StartingLevel = 0;
    uint32_t m_Level = 0;
};


