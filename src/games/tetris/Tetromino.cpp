#include <games/tetris/Tetromino.hpp>
#include <stdlib.h>

// layout of a left_aligned_bitmap: xxxx000000000000
// layout of a board_bitmap is 111xxxxxxxxxx111
static const uint16_t s_left_aligned_bitmaps[7][4][4] = {
    {
        // O piece
        {0x0000, 0x6000, 0x6000, 0x0000}, // orientation 0
        {0x0000, 0x6000, 0x6000, 0x0000}, // orientation 1
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

static std::uniform_int_distribution<int32_t> s_dist(0, TETROMINO_ID_COUNT-1);

std::mt19937& Tetromino::GetRng() {
    uint32_t seed = std::random_device()();
    static std::mt19937 rng(seed);
    return rng;
}


Tetromino::Tetromino() :
        m_Id(s_dist(GetRng())),
        m_Orientation(0),
        m_X(7),
        m_Y(19)
{
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
    uint64_t tetromino_bits = *(uint64_t*)(&s_left_aligned_bitmaps[id][orientation][0]) >> x;
    uint64_t board_bits = *(uint64_t*)(&board_bitmap[y]);
    bool is_collision = tetromino_bits & board_bits;
    return is_collision;
}

void Tetromino::Draw(int32_t level, RenderGroup& render_group) {
    float world_width = 4.0f;
    float world_height = 3.0f;
    float tetromino_size_with_border = world_height / 20.0f;
    float tetromino_size = 0.8f * tetromino_size_with_border;
    float tetromino_offset = 0.1f * tetromino_size_with_border;

    const uint16_t *left_aligned_bitmap = s_left_aligned_bitmaps[m_Id][m_Orientation];
    int8_t x0 = m_X - 3;
    int8_t y0 = m_Y - 3;
    float world_pos_x = ((world_width - tetromino_size_with_border*10) / 2.0f) + x0 * tetromino_size_with_border;
    float world_pos_y = y0 * tetromino_size_with_border;
    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 4; x++) {
            if (left_aligned_bitmap[y] & (0x8000 >> x)) {
                // local space
                V2F32 local_pos = {
                    x * tetromino_size_with_border + tetromino_offset,
                    y * tetromino_size_with_border + tetromino_offset
                };
                V2F32 local_dim = {tetromino_size, tetromino_size};


                // world space
                V3F32 world_pos = {
                    world_pos_x + local_pos.x,
                    world_pos_y + local_pos.y,
                    1.0f
                };
                V2F32 world_dim = local_dim;


                // Todo: view space
                // Todo: clip space


                V3F32 color = GetColor(m_Id, level);
                render_group.PushRectangle(world_pos, world_dim, color);
            }
        }
    }
}

void Tetromino::GetBitmap(uint16_t *bitmap) {
    *(uint64_t*)bitmap = *(uint64_t*)&s_left_aligned_bitmaps[m_Id][m_Orientation][0] >> m_X;
}

// Todo: make a proper colormap
V3F32 Tetromino::GetColor(uint8_t id, int32_t level) {
    V3F32 color;
    switch (id) {
        case TETROMINO_I:
        case TETROMINO_O:
        case TETROMINO_T: {
            color = V3F32(0.8f, 0.8f, 0.8f);
        } break;

        case TETROMINO_J:
        case TETROMINO_S: {
            color = V3F32(0.8, 0.2, 0.2);
        } break;

        default: {
            color = V3F32(0.2, 0.4, 0.2);
        }
    }
    return color;
}


