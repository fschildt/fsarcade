#pragma once

#include <basic/defs.hpp>
#include <basic/math.hpp>
#include <renderer/RenderGroup.hpp>
#include <random>

enum TetrominoId {
    TETROMINO_O = 0,
    TETROMINO_S,
    TETROMINO_Z,
    TETROMINO_T,
    TETROMINO_L,
    TETROMINO_J,
    TETROMINO_I,
    TETROMINO_ID_COUNT,
    TETROMINO_ID_NONE,
};

class Tetromino {
public:
    Tetromino();
    void MoveHorizontally(int32_t direction, uint16_t *board_bitmap);
    void Rotate(int32_t rotation, uint16_t *board_bitmap); // 1 = right; 3 = left
    bool MoveDown(uint16_t *board_bitmap);

    void GetBitmap(uint16_t *bitmap);
    void Draw(int32_t level, RenderGroup& render_group);
    bool CollidesWithBoard(uint16_t *board_bitmap, int32_t id, int32_t orientation, int32_t x, int32_t y);

    static V3F32 GetColor(uint8_t id, int32_t level);


private:
    static std::mt19937& GetRng();


public:
    int32_t m_Id;
    int32_t m_Orientation;
    int32_t m_X;
    int32_t m_Y;
};

