#include <games/tetris/Tetromino.hpp>
#include <random>
#include <stdlib.h>

// layout of a left_aligned_bitmap: xxxx000000000000
// layout of a board_bitmap is 111xxxxxxxxxx111
static const uint16_t s_left_aligned_bitmaps[7][4][4] = {
    {
        // O piece
        {0x6000, 0x6000, 0x0000, 0x0000}, // orientation 0
        {0x6000, 0x6000, 0x0000, 0x0000}, // orientation 1
        {0x6000, 0x6000, 0x0000, 0x0000}, // orientation 2
        {0x6000, 0x6000, 0x0000, 0x0000}, // orientation 3
    },
    {
        // S piece
        {0x6000, 0x3000, 0x0000, 0x0000}, // orientation 0
        {0x1000, 0x3000, 0x2000, 0x0000}, // orientation 1
        {0x6000, 0x3000, 0x0000, 0x0000}, // orientation 2
        {0x1000, 0x3000, 0x2000, 0x0000}, // orientation 3
    },
    {
        // Z piece
        {0x3000, 0x6000, 0x0000, 0x0000}, // orientation 0
        {0x2000, 0x3000, 0x1000, 0x0000}, // orientation 1
        {0x3000, 0x6000, 0x0000, 0x0000}, // orientation 2
        {0x2000, 0x3000, 0x1000, 0x0000}, // orientation 3
    },
    {
        // T piece
        {0x2000, 0x7000, 0x0000, 0x0000}, // orientation 0
        {0x2000, 0x6000, 0x2000, 0x0000}, // orientation 1
        {0x0000, 0x7000, 0x2000, 0x0000}, // orientation 2
        {0x2000, 0x3000, 0x2000, 0x0000}, // orientation 3
    },
    {
        // L piece
        {0x4000, 0x7000, 0x0000, 0x0000}, // orientation 0
        {0x2000, 0x2000, 0x6000, 0x0000}, // orientation 1
        {0x0000, 0x7000, 0x1000, 0x0000}, // orientation 2
        {0x3000, 0x2000, 0x2000, 0x0000}, // orientation 3
    },
    {
        // J piece
        {0x1000, 0x7000, 0x0000, 0x0000}, // orientation 0
        {0x6000, 0x2000, 0x2000, 0x0000}, // orientation 1
        {0x0000, 0x7000, 0x4000, 0x0000}, // orientation 2
        {0x2000, 0x2000, 0x3000, 0x0000}, // orientation 3
    },
    {
        // I piece
        {0x0000, 0xf000, 0x0000, 0x0000}, // orientation 0
        {0x2000, 0x2000, 0x2000, 0x2000}, // orientation 1
        {0x0000, 0xf000, 0x0000, 0x0000}, // orientation 2
        {0x2000, 0x2000, 0x2000, 0x2000}, // orientation 3
    }
};


TetrominoId Tetromino::GetRandomId() {
    static std::uniform_int_distribution<int> s_Dist(0, (int)TetrominoId::TETROMINO_ID_COUNT-1);
    static std::mt19937 s_Rng((std::random_device()()));
    TetrominoId id = static_cast<TetrominoId>(s_Dist(s_Rng));
    return id;
}

Tetromino::Tetromino(uint16_t *board_bitmap) :
    m_Id(GetRandomId()),
    m_Pos{6, 20},
    m_Ori{0},
    m_BoardBitmap(board_bitmap)
{
}

Tetromino::Tetromino(Board &board) :
    Tetromino(board.m_Bitmap)
{
}

TetrominoId Tetromino::GetId() {
    return m_Id;
}

BoardPos Tetromino::GetPos() {
    return m_Pos;
}

void Tetromino::GetBitmap(uint16_t *bitmap) {
    GetBitmap(m_Id, m_Pos, m_Ori, bitmap);
}

void Tetromino::MaybeRotate(TetrominoRotation rotation) {
    int32_t rot = static_cast<int32_t>(rotation);
    int32_t ori = (m_Ori + rot) % 4;
    if (!IsCollisionWithBoard(m_Id, m_Pos, ori, m_BoardBitmap)) {
        m_Ori = ori;
    }
}

void Tetromino::MaybeMoveHorizontally(TetrominoDirection direction) {
    BoardPos pos = m_Pos;
    pos.x += static_cast<int32_t>(direction);
    if (!IsCollisionWithBoard(m_Id, pos, m_Ori, m_BoardBitmap)) {
        m_Pos.x = pos.x;
    }
}

bool Tetromino::MaybeMoveDown() {
    BoardPos pos = m_Pos;
    pos.y -= 1;
    if (!IsCollisionWithBoard(m_Id, pos, m_Ori, m_BoardBitmap)) {
        m_Pos.y = pos.y;
        return true;
    }
    return false;
}

void Tetromino::Draw(RenderGroup &render_group) const {
    float world_width = 4.0f;
    float world_height = 3.0f;
    float tetromino_size_with_border = world_height / 20.0f;

    float x0 = static_cast<float>(m_Pos.x - 3);
    float y0 = static_cast<float>(m_Pos.y - 2);

    V2F32 world_pos = {
        ((world_width - tetromino_size_with_border*10) / 2.0f) + x0 * tetromino_size_with_border,
        y0 * tetromino_size_with_border
    };

    Tetromino::Draw(m_Id, m_Ori, world_pos, 1.0f, render_group);
}

bool Tetromino::IsCollisionWithBoard(TetrominoId id, BoardPos pos, int32_t ori, uint16_t *board_bitmap) {
    uint16_t tetromino_bitmap[16];
    GetBitmap(id, pos, ori, tetromino_bitmap);

    uint64_t tetromino_bits = *(uint64_t*)(tetromino_bitmap);
    uint64_t board_bits = *(uint64_t*)(&board_bitmap[pos.y]);
    bool is_collision = tetromino_bits & board_bits;
    return is_collision;
}

void Tetromino::GetBitmap(TetrominoId id, BoardPos pos, int32_t ori, uint16_t *bitmap) {
    size_t id_ = static_cast<size_t>(id);
    uint64_t *src  = (uint64_t*)s_left_aligned_bitmaps[id_][ori];
    uint64_t *dest = (uint64_t*)bitmap;
    *dest = *src >> pos.x;
}

V3F32 Tetromino::GetColor(TetrominoId id) {
    using enum TetrominoId;

    V3F32 color;
    switch (id) {
        case TETROMINO_I:
        case TETROMINO_O:
        case TETROMINO_T: {
            color = V3F32(0.8f, 0.8f, 0.8f);
        } break;

        case TETROMINO_J:
        case TETROMINO_S: {
            color = V3F32(0.8f, 0.2f, 0.2f);
        } break;

        default: {
            color = V3F32(0.2f, 0.4f, 0.2f);
        }
    }
    return color;
}

void Tetromino::Draw(TetrominoId id, int32_t ori, V2F32 pos, float scale, RenderGroup &render_group) {
    int32_t id_ = static_cast<int32_t>(id);

    float world_height = 3.0f;
    float tetromino_size_with_border = scale * world_height / 20.0f;
    float tetromino_size = 0.8f * tetromino_size_with_border;
    float tetromino_offset = 0.1f * tetromino_size_with_border;

    uint16_t *left_aligned_bitmap = (uint16_t*)s_left_aligned_bitmaps[id_][ori];
    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 4; x++) {
            if (left_aligned_bitmap[y] & (0x8000 >> x)) {
                V2F32 local_pos = {
                    (float)x * tetromino_size_with_border + tetromino_offset,
                    (float)y * tetromino_size_with_border + tetromino_offset
                };
                V2F32 local_dim = {tetromino_size, tetromino_size};


                V3F32 world_pos = {
                    pos.x + local_pos.x,
                    pos.y + local_pos.y,
                    1.0f
                };
                V2F32 world_dim = local_dim;


                V3F32 color = GetColor(id);
                render_group.PushRectangle(world_pos, world_dim, color);
            }
        }
    }
}

