#pragma once

#include <basic/defs.hpp>
#include <renderer/RenderGroup.hpp>
#include <games/tetris/Tetromino.hpp>

class Board {
public:
    Board();

    void PlaceTetromino(Tetromino tetromino);
    int32_t ClearRows(int32_t y0);
    void Draw(int32_t level, RenderGroup& render_group);

public:
    uint16_t m_Bitmap[23];
    uint8_t m_Idmap[20][10];
};

