#pragma once

#include <basic/defs.hpp>
#include <renderer/RenderGroup.hpp>
#include <games/tetris/Tetromino.hpp>

struct BoardUpdateResult {
    bool tetromino_placed;
    int cleared_rows;
};

class Board {
public:
    void Init();
    BoardUpdateResult Update(int32_t drotation, int32_t dx, int32_t dy);
    void SetTetromino(Tetromino tetromino);
    void Draw(int32_t level, RenderGroup& render_group);

private:
    void PlaceTetromino();
    int32_t ClearRows();

private:
    Tetromino m_Tetromino;
    uint16_t m_Bitmap[23];
    uint8_t m_TetrominoIdMap[200];
};

