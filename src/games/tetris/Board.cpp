#include <renderer/RenderGroup.hpp>
#include <games/tetris/Tetromino.hpp>
#include <games/tetris/Board.hpp>

Board::Board() {
    for (int y = 0; y < 2; y++) {
        m_Bitmap[y] = 0xffff; // 1111111111111111
    }
    for (int y = 2; y < 24; y++) {
        m_Bitmap[y] = 0xe007; // 1110000000000111
    }

    for (int y = 0; y < 22; y++) {
        for (int x = 0; x < 10; x++) {
            m_Idmap[y][x] = TETROMINO_ID_NONE;
        }
    }
}

void Board::PlaceTetromino(Tetromino tetromino) {
    int32_t tetromino_bitmap_x = tetromino.m_X;
    int32_t tetromino_bitmap_y = tetromino.m_Y;


    // place in Board's Bitmap
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
                int32_t idmap_y = tetromino_bitmap_y + y - 2;
                m_Idmap[idmap_y][idmap_x] = tetromino.m_Id;
            }
        }
    }
}

int32_t Board::ClearRows(int32_t y0) {
    int32_t rows_cleared = 0;
    int32_t y1 = y0 + 4;

    // ignore for y = {0,1}. Those bits are all 1's for collision testing
    if (y0 < 2) {
        y0 += 2 - y0;
    }

    for (int32_t y = y0; y < y1; y++) {
        if (m_Bitmap[y] == 0xffff) {
            rows_cleared++;
        }
        else {
            m_Bitmap[y-rows_cleared] = m_Bitmap[y];
            for (int32_t x = 0; x < 10; x++) {
                m_Idmap[y-2-rows_cleared][x] = m_Idmap[y-2][x];
            }
        }
    }
    for (int32_t y = y1; y < 24; y++) {
        m_Bitmap[y-rows_cleared] = m_Bitmap[y];
        for (int32_t x = 0; x < 10; x++) {
            m_Idmap[y-2-rows_cleared][x] = m_Idmap[y-2][x];
        }
    }
    for (int32_t y = 24-rows_cleared; y < 24; y++) {
        m_Bitmap[y] = 0xe007;
        for (int32_t x = 0; x < 10; x++) {
            m_Idmap[y-2][x] = TETROMINO_ID_NONE;
        }
    }


    return rows_cleared;
}

void Board::Draw(int32_t level, RenderGroup& render_group) {
    float world_width = 4.0f;
    float world_height = 3.0f;
    float tetromino_size_with_border = world_height / 20.0f;
    float tetromino_size = 0.8f * tetromino_size_with_border;
    float tetromino_offset = 0.1f * tetromino_size_with_border;
    V2F32 board_world_pos = {
        (world_width - tetromino_size_with_border*10) / 2.0f,
        0.0f
    };


    // background
    V3F32 bg_world_pos = {
        board_world_pos.x,
        board_world_pos.y,
        0.0f
    };
    V2F32 bg_world_dim = {
        tetromino_size_with_border * 10,
        tetromino_size_with_border * 20
    };
    V3F32 bg_color = {0.0f, 0.0f, 0.0f};
    render_group.PushRectangle(bg_world_pos, bg_world_dim, bg_color);


    // tetromino parts
    for (size_t y = 0; y < 20; y++) {
        for (size_t x = 0; x < 10; x++) {
            uint8_t tetromino_id = m_Idmap[y][x];
            if (tetromino_id < TETROMINO_ID_COUNT) {
                V2F32 local_pos = {
                    (float)x * tetromino_size_with_border + tetromino_offset,
                    (float)y * tetromino_size_with_border + tetromino_offset
                };
                V2F32 local_dim = {tetromino_size, tetromino_size};


                V3F32 world_pos = {
                    board_world_pos.x + local_pos.x,
                    board_world_pos.y + local_pos.y,
                    1.0f
                };
                V2F32 world_dim = local_dim;


                V3F32 color = Tetromino::GetColor(tetromino_id, level);
                render_group.PushRectangle(world_pos, world_dim, color);
            }
        }
    }
}

