#include <games/tetris/PlayerState.h>
#include <games/Game.h>

#include <renderer/RenderGroup.h>

#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

void PlayerState::Init() {
    for (int i = 0; i < 3; i++) {
        m_LineOccupations[i] = 0xffff;
    }
    for (int i = 3; i < 23; i++) {
        m_LineOccupations[i] = 0xe007;
    }
    m_ActiveTetromino.InitRandomly();
    m_NextTetromino.InitRandomly();

    // for debug purposes
    V3 init_color = V3(1.0, 1.0, 1.0);
    for (int i = 0; i < 200; i++) {
        m_Colors[i] = init_color;
    }

    srand(time(0));
}

void PlayerState::Update(const GameController *controller, RenderGroup *render_group) {
    // rotation
    uint16_t drotation = 0;
    if (controller->ActionA) {
        drotation = 3;
    } else if (controller->ActionB) {
        drotation = 1;
    }

    // x translation
    int8_t dx = 0;
    if (controller->MoveLeft) {
        dx = -1;
    } else if (controller->MoveRight) {
        dx = 1;
    }

    // y translation
    int8_t dy = controller->MoveDown ? -1 : 0;
    static int counter = 0;
    if (counter++ == 60) {
        dy -= 1;
        counter = 0;
    }

    bool updated = m_ActiveTetromino.Update(dx, dy, drotation, m_LineOccupations);
    if (!updated) {
        PlaceActiveTetromino();
        m_ActiveTetromino = m_NextTetromino;
        m_NextTetromino.InitRandomly();
    }

    Draw(render_group);
}

void PlayerState::Draw(RenderGroup *render_group) {
    m_ActiveTetromino.Draw(render_group);
    for (int y = 3; y < 23; y++) {
        for (int x = 3; x < 13; x++) {
            uint16_t bit_tester = 0x8000 >> x;
            int x_color = x - 3;
            int y_color = y - 3;
            if (m_LineOccupations[y] & bit_tester) {
                V3 pos(x_color + 0.1, y_color + 0.1, 1);
                V2 dim(0.8, 0.8);
                V3 *color = &m_Colors[y_color * 10 + x_color];
                render_group->PushRectangle(pos, dim, *color);
            } else {
#if 0
                V3 pos(x_color + 0.1, y_color + 0.1, 0);
                V2 dim(0.8, 0.8);
                V3 color = V3(0.5f, 0.1f, 0.1);
                render_group->PushRectangle(pos, dim, color);
#endif

            }
        }
    }
}

void PlayerState::PlaceActiveTetromino() {
    uint16_t tetromino_line_occupations[4];
    m_ActiveTetromino.GetLineOccupations(tetromino_line_occupations);

    int8_t tetromino_x = m_ActiveTetromino.GetX();
    int8_t tetromino_y = m_ActiveTetromino.GetY();
    m_LineOccupations[tetromino_y+0] |= tetromino_line_occupations[0];
    m_LineOccupations[tetromino_y+1] |= tetromino_line_occupations[1];
    m_LineOccupations[tetromino_y+2] |= tetromino_line_occupations[2];
    m_LineOccupations[tetromino_y+3] |= tetromino_line_occupations[3];


    V3 color = m_ActiveTetromino.GetColor();
    for (int8_t y = 0; y < 4; y++) {
        for (int8_t x = 0; x <= 4; x++) {
            int16_t test_value = 0x8000 >> (tetromino_x + x);
            if (tetromino_line_occupations[y] & test_value) {
                int16_t color_x = tetromino_x + x - 3;
                int16_t color_y = tetromino_y + y - 3;
                m_Colors[color_y*10 + color_x] = color;
            }
        }
    }

    ClearFullLines(tetromino_y);
}

void PlayerState::ClearFullLines(int16_t y0) {
    int16_t y = y0;
    int16_t ymax = y0 + 3;
    if (y <= 2) {
        int16_t dy = 3 - y0;
        y += dy;
        ymax -= dy;
    }

    int clear_count = 0;
    while (y <= ymax) {
        if (m_LineOccupations[y] == 0xffff) {
            clear_count++;
        } else if(clear_count) {
            ClearFullLinesPart(y-clear_count, clear_count);
            y -= clear_count;
            clear_count = 0;
        }
        y++;
    }
    if (clear_count) {
        ClearFullLinesPart(y-clear_count, clear_count);
    }
}

void PlayerState::ClearFullLinesPart(int16_t y0, int clear_count) {
    void *dest;
    void *src;
    size_t size;

    int drop_count = 23 - y0 - 1;

    // move lines down
    dest = &m_LineOccupations[y0];
    src  = &m_LineOccupations[y0 + clear_count];
    size = drop_count * sizeof(m_LineOccupations[0]);
    memmove(dest, src, size);

    // move colors down
    dest = &m_Colors[10*(y0-3)];
    src  = &m_Colors[10*(y0-3+clear_count)];
    size = drop_count * 10*sizeof(m_Colors[0]);
    memmove(dest, src, size);

    // reinit lines at top
    int16_t ytop = sizeof(m_LineOccupations) / sizeof(m_LineOccupations[0]) - 1;
    for (int y = ytop-clear_count+1; y <= ytop; y++) {
        m_LineOccupations[y] = 0xe007; // 1110000000000111
    }
}

