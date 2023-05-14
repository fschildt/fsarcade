#include <games/tetris/PlayerState.h>
#include <games/Game.h>

#include <renderer/RenderGroup.h>

#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

void PlayerState::Init() {
    srand(time(0));

    for (int i = 0; i < 3; i++) {
        m_LineOccupations[i] = 0xffff; // 1111111111111111
    }
    for (int i = 3; i < 23; i++) {
        m_LineOccupations[i] = 0xe007; // 1110000000000111
    }
    m_ActiveTetromino.InitRandomly();
    m_NextTetromino.InitRandomly();

    m_StartLevel = 0;
    m_CurrentLevel = 0;
    m_Score = 0;
    m_ClearedLines = 0;
    for (auto piece_count : m_PieceCounts) {
        piece_count = 0;
    }
}

uint32_t PlayerState::GetDropSpeedInMilliseconds() {
    uint32_t drop_speed;

    // Todo: faster runtime with making a table and finding the index
    uint16_t level = m_CurrentLevel;
    if (level == 0) drop_speed = 400;
    else if (level == 1) drop_speed = 375;
    else if (level == 2) drop_speed = 350;
    else if (level == 3) drop_speed = 325;
    else if (level == 4) drop_speed = 300;
    else if (level == 5) drop_speed = 275;
    else if (level == 6) drop_speed = 250;
    else if (level == 7) drop_speed = 225;
    else if (level == 8) drop_speed = 200;
    else if (level == 9) drop_speed = 180;
    else if (level >= 10 && level <= 12) drop_speed = 150;
    else if (level >= 13 && level <= 15) drop_speed = 120;
    else if (level >= 16 && level <= 18) drop_speed = 90;
    else if (level >= 19 && level <= 28) drop_speed = 60;
    else drop_speed = 60;

    return drop_speed;
}

void PlayerState::Update(const GameController *controller, uint64_t dt, RenderGroup *render_group) {
    // find drotation
    uint16_t drotation = 0;
    if (controller->ActionA) {
        drotation = 3;
    } else if (controller->ActionB) {
        drotation = 1;
    }

    // find dx
    int8_t dx = 0;
    if (controller->MoveLeft) {
        dx = -1;
    } else if (controller->MoveRight) {
        dx = 1;
    }

    // find dy
    int8_t dy = 0;
    static uint64_t time_since_last_drop = 0;
    uint64_t drop_speed_in_milliseconds = GetDropSpeedInMilliseconds();
    time_since_last_drop += dt;
    while (time_since_last_drop >= drop_speed_in_milliseconds) {
        time_since_last_drop -= drop_speed_in_milliseconds;
        dy -= 1;
    }
    if (controller->MoveDown) {
        dy -= 1;
    }

    // update
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
    m_CurrentLevel = m_StartLevel + (m_ClearedLines / 10);
}

void PlayerState::ClearFullLines(int16_t y) {
    int16_t yoff = 0;
    int16_t ymax;
    if (y <= 2) {
        yoff = 3 - y;
    }

    y += yoff;
    ymax = y + 3 - yoff;

    int clear_count = 0;
    while (y <= ymax || clear_count) {
        if (m_LineOccupations[y] == 0xffff) {
            clear_count++;
        } else if(clear_count) {
            ClearFullLinesPart(y-clear_count, clear_count);
            y -= clear_count;
            m_ClearedLines += clear_count;
            clear_count = 0;
        }
        y++;
    }
}

void PlayerState::ClearFullLinesPart(int16_t y0, int clear_count) {
    void *dest;
    void *src;
    size_t size;

    int drop_count = 23 - y0 - 1;

    // Todo: use own memory (see Tetris::Tetris(...)) instead of memmove

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

