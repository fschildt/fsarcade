#pragma once

#include <basic/defs.hpp>
#include <renderer/RenderGroup.hpp>

class Tetromino;

struct BoardPos {
    int32_t x;
    int32_t y;
};

class Board {
public:
    Board();

    int32_t PlaceTetromino(Tetromino &tetromino);
    void Draw(int32_t level, RenderGroup& render_group);


private:
    int32_t ClearRows(int32_t y0);


private:
    friend class Tetromino;
    uint16_t m_Bitmap[24];
    uint8_t m_Idmap[22][10];
};

