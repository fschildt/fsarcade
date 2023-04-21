#pragma once

#include "renderer/RenderGroup.h"
#include <math/math.h>

class Tetromino {
public:
    void InitRandomly();
    bool Update(int8_t dx, int8_t dy, uint16_t drotation, uint16_t field_line_occupations[20]);
    void Draw(RenderGroup *render_group);
    void GetLineOccupations(uint16_t occupation[4]);

public:
    int GetX();
    int GetY();
    V3 GetColor();

private:
    bool CanOccupy(int8_t x, int8_t y, uint16_t rotation, uint16_t field_occupations[20]);
    void GetLineOccupations(int piece, int rotation, uint16_t occupations[4], int x);

private:

    enum PieceIndex : uint16_t {
        PIECE_O = 0,
        PIECE_S,
        PIECE_Z,
        PIECE_T,
        PIECE_L,
        PIECE_J,
        PIECE_I,
        PIECE_COUNT
    };

    int m_PieceIndex;
    uint16_t m_RotationIndex;
    int8_t m_X;
    int8_t m_Y;
    V3 m_Color;
};

