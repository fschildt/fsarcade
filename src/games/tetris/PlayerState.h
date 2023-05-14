#pragma once

#include <games/tetris/Tetromino.h>
class RenderGroup;
struct GameController;

class PlayerState {
public:
    void Update(const GameController *controller, uint64_t dt, RenderGroup *render_group);
    void Init();

private:
    void Draw(RenderGroup *render_group);
    void PlaceActiveTetromino();
    void ClearFullLines(int16_t y0);
    void ClearFullLinesPart(int16_t y0, int clear_count);
    uint32_t GetDropSpeedInMilliseconds();

private:
    Tetromino m_NextTetromino;
    Tetromino m_ActiveTetromino;
    uint16_t m_LineOccupations[23];
    V3 m_Colors[200]; // Todo: Maybe replace this with int16_t indices[200] and make a color table
    uint16_t m_StartLevel;
    uint16_t m_CurrentLevel;
    uint16_t m_Score;
    uint16_t m_ClearedLines;
    uint16_t m_PieceCounts[Tetromino::PIECE_COUNT];
};

