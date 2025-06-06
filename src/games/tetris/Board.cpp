#include <renderer/RenderGroup.hpp>
#include <games/tetris/Tetromino.hpp>
#include <games/tetris/Board.hpp>

void Board::Init() {
    for (int y = 0; y < 3; y++) {
        m_Bitmap[y] = 0xffff; // 1111111111111111
    }
    for (int y = 3; y < 23; y++) {
        m_Bitmap[y] = 0xe007; // 1110000000000111
    }

    for (int y = 0; y < 20; y++) {
        for (int x = 0; x < 10; x++) {
            m_Idmap[y][x] = TETROMINO_ID_NONE;
        }
    }
}

void Board::PlaceTetromino(Tetromino tetromino) {
    int32_t tetromino_bitmap_x = tetromino.m_X;
    int32_t tetromino_bitmap_y = tetromino.m_Y;


    // place in Board's Bitmap
    // Todo: use SIMD?
    uint16_t tetromino_bitmap[4];
    tetromino.GetBitmap(tetromino_bitmap);

    m_Bitmap[tetromino_bitmap_y+0] |= tetromino_bitmap[0];
    m_Bitmap[tetromino_bitmap_y+1] |= tetromino_bitmap[1];
    m_Bitmap[tetromino_bitmap_y+2] |= tetromino_bitmap[2];
    m_Bitmap[tetromino_bitmap_y+3] |= tetromino_bitmap[3];


    // place in Board's Idmap
    for (int32_t y = 0; y < 4; y++) {
        for (int32_t x = 0; x < 4; x++) {
            int32_t bitmap_x = 0x8000 >> (tetromino_bitmap_x + x);
            if (tetromino_bitmap[y] & bitmap_x) {
                int32_t idmap_x = tetromino_bitmap_x + x - 3;
                int32_t idmap_y = tetromino_bitmap_y + y - 3;
                m_Idmap[idmap_y][idmap_x] = tetromino.m_Id;
            }
        }
    }
}

int32_t Board::ClearRows(int32_t y0) {
    int32_t rows_cleared = 0;
    int32_t y1 = y0 + 4;

    // ignore for y = {0,1,2}. Those bits are all 1's for collision testing
    if (y0 < 3) {
        y0 += 3 - y0;
    }

    for (int32_t y = y0; y < y1; y++) {
        if (m_Bitmap[y] == 0xffff) {
            rows_cleared++;
        }
        else {
            m_Bitmap[y-rows_cleared] = m_Bitmap[y];
            for (int32_t x = 0; x < 10; x++) {
                m_Idmap[y-3-rows_cleared][x] = m_Idmap[y-3][x];
            }
        }
    }
    for (int32_t y = y1; y < 23; y++) {
        m_Bitmap[y-rows_cleared] = m_Bitmap[y];
        for (int32_t x = 0; x < 10; x++) {
            m_Idmap[y-3-rows_cleared][x] = m_Idmap[y-3][x];
        }
    }
    for (int32_t y = 22-rows_cleared; y < 23; y++) {
        m_Bitmap[y] = 0xe007;
        for (int32_t x = 0; x < 10; x++) {
            m_Idmap[y-3][x] = TETROMINO_ID_NONE;
        }
    }


    return rows_cleared;
}

void Board::Draw(int32_t level, RenderGroup& render_group) {
    for (size_t y = 0; y < 20; y++) {
        for (size_t x = 0; x < 10; x++) {
            uint8_t tetromino_id = m_Idmap[y][x];
            if (tetromino_id < TETROMINO_ID_COUNT) {
                V3 pos = {x + 0.1f, y + 0.1f, 1};
                V2 dim = {0.8f, 0.8f};
                V3 color = Tetromino::GetColor(tetromino_id, level);
                render_group.PushRectangle(pos, dim, color);
            }
        }
    }
}

