#include <renderer/RenderGroup.hpp>
#include <games/tetris/Tetromino.hpp>
#include <games/tetris/Board.hpp>

#include <stdio.h>

void Board::Init() {
    for (int y = 0; y < 3; y++) {
        m_Bitmap[y] = 0xffff; // 1111111111111111
    }
    for (int y = 3; y < 23; y++) {
        m_Bitmap[y] = 0xe007; // 1110000000000111
    }
}

BoardUpdateResult Board::Update(int32_t drotation, int32_t dx, int32_t dy) {
    BoardUpdateResult result = {0, false};

    if (!m_Tetromino.Update(m_Bitmap, drotation, dx, dy)) {
        PlaceTetromino();
        int32_t cleared_rows = ClearRows();

        result.tetromino_placed = true;
        result.cleared_rows = cleared_rows;
    }

    return result;
}

void Board::SetTetromino(Tetromino tetromino) {
    m_Tetromino = tetromino;
}

void Board::PlaceTetromino() {
    uint16_t tetromino_bitmap[4];
    m_Tetromino.GetBitmap(tetromino_bitmap);

    int32_t tetromino_x = m_Tetromino.m_X;
    int32_t tetromino_y = m_Tetromino.m_Y;

    // @RESEARCH: SIMD?
    m_Bitmap[tetromino_y+0] |= tetromino_bitmap[0];
    m_Bitmap[tetromino_y+1] |= tetromino_bitmap[1];
    m_Bitmap[tetromino_y+2] |= tetromino_bitmap[2];
    m_Bitmap[tetromino_y+3] |= tetromino_bitmap[3];

    for (int32_t y = 0; y < 4; y++) {
        for (int32_t x = 0; x < 4; x++) {
            uint16_t test_bit = 0x8000 >> (tetromino_x + x);
            if (tetromino_bitmap[y] & test_bit) {
                int32_t id_x = tetromino_x + x - 3;
                int32_t id_y = tetromino_y + y - 3;
                m_TetrominoIdMap[id_y*10 + id_x] = m_Tetromino.m_Id;
            }
        }
    }
}


int32_t Board::ClearRows() {
    uint16_t *bitmap_row_max = &m_Bitmap[m_Tetromino.m_Y + 3];
    uint16_t *bitmap_row_dest = m_Tetromino.m_Y >= 3 ? &m_Bitmap[m_Tetromino.m_Y] : &m_Bitmap[3];
    uint16_t *bitmap_row_curr = bitmap_row_dest;

    uint8_t *tetromino_id_dest = m_Tetromino.m_Y >= 3 ? &m_TetrominoIdMap[(m_Tetromino.m_Y-3)*10] : &m_TetrominoIdMap[0];
    uint8_t *tetromino_id_curr = tetromino_id_dest;

    // drop full rows within 4 rows
    while (bitmap_row_curr <= bitmap_row_max) {
        if (*bitmap_row_curr == 0xffff) {
            bitmap_row_curr++;
            tetromino_id_curr += 10;
        } else {
            for (int i = 0; i < 10; i++) {
                *tetromino_id_dest++ = *tetromino_id_curr++;
            }
            *bitmap_row_dest++ = *bitmap_row_curr++;
        }
    }
    int32_t dropcount = bitmap_row_curr - bitmap_row_dest;

    // drop reamining rows above the tested 4 rows
    bitmap_row_max = &m_Bitmap[22];
    while (bitmap_row_curr <= bitmap_row_max) {
        for (int i = 0; i < 10; i++) {
            *tetromino_id_dest++ = *tetromino_id_curr++;
        }
        *bitmap_row_dest++ = *bitmap_row_curr++;
    }

    // reinit bitmap at top (ids can be ignored)
    bitmap_row_curr = &m_Bitmap[22-dropcount];
    while (bitmap_row_curr <= bitmap_row_max) {
        *bitmap_row_curr++ = 0xe007; // 1110000000000111
    }

    return dropcount;
}

void Board::Draw(int32_t level, RenderGroup& render_group) {
    uint16_t *bitmap_row = m_Bitmap + 3;
    for (int y = 0; y < 20; y++) {
        uint16_t bit_test = 0x8000 >> 3; // starting from index 3 in 111xxxxxxxxxx111
        for (int x = 0; x < 10; x++) {
            if (*bitmap_row & bit_test) {
                V3 pos = {x + 0.1f, y + 0.1f, 1};
                V2 dim = {0.8f, 0.8f};
                uint8_t tetromino_id = m_TetrominoIdMap[y*10+x];
                V3 color = Tetromino::GetColor(tetromino_id, level);
                render_group.PushRectangle(pos, dim, color);
            }
            bit_test >>= 1;
        }
        bitmap_row++;
    }

    m_Tetromino.Draw(level, render_group);
}

