#pragma once

#include <imgui.h>
#include <games/Game.hpp>
#include <games/tetris/Tetromino.hpp>
#include <games/tetris/Board.hpp>
#include <renderer/RenderGroup.hpp>

enum class TetrisRunningState {
    Resume,
    Pause,
    GameOver,
    Restart,
    Exit
};


class Tetris : public Game {
public:
    Tetris();
    bool Update(std::vector<SDL_Event> &events, RenderGroup& render_group) override;
    void HandleTetrominoPlacement();

private:
    void Restart();
    void UpdateResumeState(SDL_Event &event);
    void UpdatePauseState(SDL_Event &event);

    uint32_t GetHarddropCount(float dt);

    TetrisRunningState Draw(RenderGroup &render_group);
    void DrawLineCounter(RenderGroup &render_group);
    void DrawStatistics(RenderGroup &render_group);
    void DrawScore(RenderGroup &render_group);
    void DrawNextTetromino(RenderGroup &render_group);
    void DrawLevel(RenderGroup &render_group);

    TetrisRunningState DrawPauseMenu(RenderGroup &render_group);
    TetrisRunningState DrawGameOverMenu(RenderGroup &render_group);



private:
    TetrisRunningState m_RunningState = TetrisRunningState::Resume;

    float m_DtInSecondsRemaining = 0.0f;
    uint64_t m_MillisecondsSinceT0Last = SDL_GetTicks();

    Board m_Board;
    Tetromino m_ActiveTetromino;
    Tetromino m_NextTetromino;

    int32_t m_TetrominoCounters[(size_t)TetrominoId::TETROMINO_ID_COUNT] {};
    int32_t m_Score = 0;
    int32_t m_LineCounter = 0;
    int32_t m_StartingLevel = 0;
    int32_t m_Level = 0;
    int32_t m_SoftdropCounter = 0;

    int32_t m_HighScore = 0;

    ImGuiWindowFlags m_ImGuiWindowFlags = ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoScrollbar;
};


