#pragma once

#include <defs.hpp>
#include <math/math.hpp>
#include <renderer/RenderGroup.hpp>

enum TetrominoId {
    TETROMINO_O = 0,
    TETROMINO_S,
    TETROMINO_Z,
    TETROMINO_T,
    TETROMINO_L,
    TETROMINO_J,
    TETROMINO_I,
    TETROMINO_ID_COUNT
};

class Tetromino {
public:
    void Init();
    bool Update(uint16_t *board_bitmap, int32_t drotation, int32_t dx, int32_t dy);
    void Draw(int32_t level, RenderGroup *render_group);
    void GetBitmap(uint16_t *bitmap);
    static V3 GetColor(uint8_t id, int32_t level);

private:
    static bool CollidesWithBoard(uint16_t *board_bitmap, int32_t id, int32_t rotation, int32_t x, int32_t y);

public:
    int32_t m_Id;
    int32_t m_Orientation;
    int32_t m_X;
    int32_t m_Y;
};

