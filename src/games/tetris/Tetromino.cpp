#include <games/tetris/Tetromino.hpp>
#include <stdlib.h>

// layout of a left_aligned_bitmap: xxxx000000000000
// layout of a board_bitmap is 111xxxxxxxxxx111
static const uint16_t s_left_aligned_bitmaps[7][4][4] = {
    {
        // O piece
        {0x0000, 0x6000, 0x6000, 0x0000}, // orientation 0
        {0x0000, 0x6000, 0x6000, 0x0000}, // orientation 0
        {0x0000, 0x6000, 0x6000, 0x0000}, // orientation 2
        {0x0000, 0x6000, 0x6000, 0x0000}, // orientation 3
    },
    {
        // S piece
        {0x0000, 0x6000, 0x3000, 0x0000}, // orientation 0
        {0x0000, 0x1000, 0x3000, 0x2000}, // orientation 1
        {0x0000, 0x6000, 0x3000, 0x0000}, // orientation 2
        {0x0000, 0x1000, 0x3000, 0x2000}, // orientation 3
    },
    {
        // Z piece
        {0x0000, 0x3000, 0x6000, 0x0000}, // orientation 0
        {0x0000, 0x2000, 0x3000, 0x1000}, // orientation 1
        {0x0000, 0x3000, 0x6000, 0x0000}, // orientation 2
        {0x0000, 0x2000, 0x3000, 0x1000}, // orientation 3
    },
    {
        // T piece
        {0x0000, 0x2000, 0x7000, 0x0000}, // orientation 0
        {0x0000, 0x2000, 0x6000, 0x2000}, // orientation 1
        {0x0000, 0x0000, 0x7000, 0x2000}, // orientation 2
        {0x0000, 0x2000, 0x3000, 0x2000}, // orientation 3
    },
    {
        // L piece
        {0x0000, 0x4000, 0x7000, 0x0000}, // orientation 0
        {0x0000, 0x2000, 0x2000, 0x6000}, // orientation 1
        {0x0000, 0x0000, 0x7000, 0x1000}, // orientation 2
        {0x0000, 0x3000, 0x2000, 0x2000}, // orientation 3
    },
    {
        // J piece
        {0x0000, 0x1000, 0x7000, 0x0000}, // orientation 0
        {0x0000, 0x6000, 0x2000, 0x2000}, // orientation 1
        {0x0000, 0x0000, 0x7000, 0x4000}, // orientation 2
        {0x0000, 0x2000, 0x2000, 0x3000}, // orientation 3
    },
    {
        // I piece
        {0x0000, 0xf000, 0x0000, 0x0000}, // orientation 0
        {0x2000, 0x2000, 0x2000, 0x2000}, // orientation 1
        {0x0000, 0xf000, 0x0000, 0x0000}, // orientation 2
        {0x2000, 0x2000, 0x2000, 0x2000}, // orientation 3
    }
};



void Tetromino::Init() {
    m_Id = rand() % TETROMINO_ID_COUNT;
    m_Orientation = 0;
    m_X = 7;
    m_Y = 19;
}

void Tetromino::MoveHorizontally(int32_t direction, uint16_t *board_bitmap) {
    int32_t desired_x = m_X + direction;
    if (!CollidesWithBoard(board_bitmap, m_Id, m_Orientation, desired_x, m_Y)) {
        m_X = desired_x;
    }
}

void Tetromino::Rotate(int32_t rotation, uint16_t *board_bitmap) {
    int32_t desired_ori = (m_Orientation + rotation) % 4;
    if (!CollidesWithBoard(board_bitmap, m_Id, desired_ori, m_X, m_Y)) {
        m_Orientation = desired_ori;
    }
}

bool Tetromino::MoveDown(uint16_t *board_bitmap) {
    int32_t target_y = m_Y - 1;
    if (!CollidesWithBoard(board_bitmap, m_Id, m_Orientation, m_X, target_y)) {
        m_Y = target_y;
        return true;
    }
    return false;
}

bool Tetromino::CollidesWithBoard(uint16_t *board_bitmap, int32_t id, int32_t orientation, int32_t x, int32_t y) {
    // Todo: fix unaligned uint64_t access; just test 4 times uint16_t, maybe even with SIMD
    uint64_t collision_testee = *(uint64_t*)(&s_left_aligned_bitmaps[id][orientation][0]) >> x;
    bool shares_bits = collision_testee & *(uint64_t*)(&board_bitmap[y]);
    return shares_bits;
}

void Tetromino::Draw(int32_t level, RenderGroup& render_group) {
    uint16_t left_aligned_bitmap[4];
    left_aligned_bitmap[0] = s_left_aligned_bitmaps[m_Id][m_Orientation][0];
    left_aligned_bitmap[1] = s_left_aligned_bitmaps[m_Id][m_Orientation][1];
    left_aligned_bitmap[2] = s_left_aligned_bitmaps[m_Id][m_Orientation][2];
    left_aligned_bitmap[3] = s_left_aligned_bitmaps[m_Id][m_Orientation][3];
    int8_t x0 = m_X - 3;
    int8_t y0 = m_Y - 3;
    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 4; x++) {
            if (left_aligned_bitmap[y] & (0x8000 >> x)) {
                V3 pos = V3(x0 + x + 0.1f, y0 + y + 0.1f, 2);
                V2 dim = V2(0.8, 0.8);
                V3 color = GetColor(m_Id, level);
                render_group.PushRectangle(pos, dim, color);
            }
        }
    }
}

void Tetromino::GetBitmap(uint16_t *bitmap) {
    *(uint64_t*)bitmap = *(uint64_t*)&s_left_aligned_bitmaps[m_Id][m_Orientation][0] >> m_X;
}

// Todo: make a proper colormap
V3 Tetromino::GetColor(uint8_t id, int32_t level) {
    V3 color;
    switch (id) {
        case TETROMINO_I:
        case TETROMINO_O:
        case TETROMINO_T: {
            color = V3(0.8f, 0.8f, 0.8f);
        } break;

        case TETROMINO_J:
        case TETROMINO_S: {
            color = V3(0.8, 0.2, 0.2);
        } break;

        default: {
            color = V3(0.2, 0.4, 0.2);
        }
    }
    return color;
}


