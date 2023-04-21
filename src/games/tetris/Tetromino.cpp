#include <games/tetris/Tetromino.h>
#include <math/math.h>
#include <renderer/RenderGroup.h>

#include <stdio.h>
#include <stdlib.h>

// format: [piece_index][rotation][line]
static const uint16_t s_local_occupations[7][4][4] = {
    {
        // O piece
        {0x0000, 0x6000, 0x6000, 0x0000}, // rotation 0
        {0x0000, 0x6000, 0x6000, 0x0000}, // rotation 0
        {0x0000, 0x6000, 0x6000, 0x0000}, // rotation 2
        {0x0000, 0x6000, 0x6000, 0x0000}, // rotation 3
    },
    {
        // S piece
        {0x0000, 0x6000, 0x3000, 0x0000}, // rotation 0
        {0x0000, 0x1000, 0x3000, 0x2000}, // rotation 1
        {0x0000, 0x6000, 0x3000, 0x0000}, // rotation 2
        {0x0000, 0x1000, 0x3000, 0x2000}, // rotation 3
    },
    {
        // Z piece
        {0x0000, 0x3000, 0x6000, 0x0000}, // rotation 0
        {0x0000, 0x2000, 0x3000, 0x1000}, // rotation 1
        {0x0000, 0x3000, 0x6000, 0x0000}, // rotation 2
        {0x0000, 0x2000, 0x3000, 0x1000}, // rotation 3
    },
    {
        // T piece
        {0x0000, 0x2000, 0x7000, 0x0000}, // rotation 0
        {0x0000, 0x2000, 0x6000, 0x2000}, // rotation 1
        {0x0000, 0x0000, 0x7000, 0x2000}, // rotation 2
        {0x0000, 0x2000, 0x3000, 0x2000}, // rotation 3
    },
    {
        // L piece
        {0x0000, 0x4000, 0x7000, 0x0000}, // rotation 0
        {0x0000, 0x2000, 0x2000, 0x6000}, // rotation 1
        {0x0000, 0x0000, 0x7000, 0x1000}, // rotation 2
        {0x0000, 0x3000, 0x2000, 0x2000}, // rotation 3
    },
    {
        // J piece
        {0x0000, 0x1000, 0x7000, 0x0000}, // rotation 0
        {0x0000, 0x6000, 0x2000, 0x2000}, // rotation 1
        {0x0000, 0x0000, 0x7000, 0x4000}, // rotation 2
        {0x0000, 0x2000, 0x2000, 0x3000}, // rotation 3
    },
    {
        // I piece
        {0x0000, 0xf000, 0x0000, 0x0000}, // rotation 0
        {0x2000, 0x2000, 0x2000, 0x2000}, // rotation 1
        {0x0000, 0xf000, 0x0000, 0x0000}, // rotation 2
        {0x2000, 0x2000, 0x2000, 0x2000}, // rotation 3
    }
};

void Tetromino::InitRandomly() {
    m_PieceIndex = rand() % PIECE_COUNT;
    switch (m_PieceIndex) {
        case PIECE_I:
        case PIECE_O:
        case PIECE_T: {
            m_Color = V3(0.8f, 0.8f, 0.8f);
        } break;

        case PIECE_J:
        case PIECE_S: {
            m_Color = V3(0.8, 0.2, 0.2);
        } break;

        default: {
            m_Color = V3(0.2, 0.4, 0.2);
        }
    }
    m_RotationIndex = 0;
    m_X = 7;
    m_Y = 19;
}

bool Tetromino::Update(int8_t dx, int8_t dy, uint16_t drotation, uint16_t field_line_occupations[20]) {
    // x translation
    int8_t desired_x = m_X + dx;
    if (CanOccupy(desired_x, m_Y, m_RotationIndex, field_line_occupations)) {
        m_X = desired_x;
    }

    // rotation
    uint16_t desired_rotation = (m_RotationIndex + drotation) % 4;
    if (CanOccupy(m_X, m_Y, desired_rotation, field_line_occupations)) {
        m_RotationIndex = desired_rotation;
    }

    // y translation (one step at a time)
    while (dy < 0) {
        int8_t desired_y = m_Y - 1;
        if (CanOccupy(m_X, desired_y, m_RotationIndex, field_line_occupations)) {
            m_Y = desired_y;
        } else {
            return false;
        }
        dy++;
    }

    return true;
}

void Tetromino::GetLineOccupations(uint16_t occupations[4]) {
    GetLineOccupations(m_PieceIndex, m_RotationIndex, occupations, m_X);
}

void Tetromino::Draw(RenderGroup *render_group) {
    uint16_t local_occupations[4];
    local_occupations[0] = s_local_occupations[m_PieceIndex][m_RotationIndex][0];
    local_occupations[1] = s_local_occupations[m_PieceIndex][m_RotationIndex][1];
    local_occupations[2] = s_local_occupations[m_PieceIndex][m_RotationIndex][2];
    local_occupations[3] = s_local_occupations[m_PieceIndex][m_RotationIndex][3];
    int8_t xoff = m_X - 3;
    int8_t yoff = m_Y - 3;
    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 4; x++) {
            if (local_occupations[y] & (0x8000 >> x)) {
                V3 pos = V3(x + xoff + 0.1f, y + yoff + 0.1f, 2);
                V2 dim = V2(0.8, 0.8);
                V3 color = m_Color;
                render_group->PushRectangle(pos, dim, color);
            }
        }
    }
}

int Tetromino::GetX() {
    return m_X;
}

int Tetromino::GetY() {
    return m_Y;
}

V3 Tetromino::GetColor() {
    return m_Color;
}

void Tetromino::GetLineOccupations(int piece, int rotation, uint16_t occupations[4], int x) {
    occupations[0] = s_local_occupations[piece][rotation][0];
    occupations[1] = s_local_occupations[piece][rotation][1];
    occupations[2] = s_local_occupations[piece][rotation][2];
    occupations[3] = s_local_occupations[piece][rotation][3];

    occupations[0] >>= x;
    occupations[1] >>= x;
    occupations[2] >>= x;
    occupations[3] >>= x;
}


bool Tetromino::CanOccupy(int8_t x, int8_t y, uint16_t rotation, uint16_t field_occupations[20]) {
    uint16_t desired_occupation[4];
    GetLineOccupations(m_PieceIndex, rotation, desired_occupation, x);

    uint16_t shared_occupations0 = field_occupations[y+0] & desired_occupation[0];
    uint16_t shared_occupations1 = field_occupations[y+1] & desired_occupation[1];
    uint16_t shared_occupations2 = field_occupations[y+2] & desired_occupation[2];
    uint16_t shared_occupations3 = field_occupations[y+3] & desired_occupation[3];

    bool shared = shared_occupations0 |
                  shared_occupations1 |
                  shared_occupations2 |
                  shared_occupations3;
    return !shared;
}

