#include <games/tetris/Tetris.hpp>

#include <stdio.h>

// Todo: NES rules
// - row clears: first level up, then calculate score
// - softdrop bonus only when a tetromino is placed with it.

void Tetris::Init() {
    m_Level = 0;
    m_DtRemained = 0;

    m_Board.Init();

    m_NextTetromino.Init();
    m_Board.SetTetromino(m_NextTetromino);
    m_NextTetromino.Init();
}

void Tetris::Update(GameInput& input, RenderGroup& render_group) {
    V3 clear_color = V3(0.f, 0.f, 0.f);
    render_group.SetSize(10, 20);
    render_group.PushClear(clear_color);

    int32_t drotation = 0;
    int32_t dx = 0;
    int32_t dy = 0;

    GameController *controller = &input.controller;
    if (controller->ActionA) {
        drotation = 1;
    }
    if (controller->ActionB) {
        drotation = 3;
    }
    if (controller->MoveLeft) {
        dx = -1;
    }
    if (controller->MoveRight) {
        dx = 1;
    }
    if (controller->MoveDown) {
        dy = -1;
    }
    dy -= GetHarddropCount(input.dt);


    BoardUpdateResult board_update_result = m_Board.Update(drotation, dx, dy);
    if (board_update_result.tetromino_placed) {
        m_Board.SetTetromino(m_NextTetromino);
        m_NextTetromino.Init();
    }

    // Todo: update stats/level here

    m_Board.Draw(m_Level, render_group);
}

int32_t Tetris::GetHarddropCount(float dt) {
    float nes_frame_time = 1.0f / 60;
    int nes_frames_per_cell;
    if (m_Level <= 8)       nes_frames_per_cell = 48 - m_Level * 5;
    else if (m_Level == 9)  nes_frames_per_cell = 6;
    else if (m_Level <= 12) nes_frames_per_cell = 5;
    else if (m_Level <= 15) nes_frames_per_cell = 4;
    else if (m_Level <= 18) nes_frames_per_cell = 3;
    else if (m_Level <= 28) nes_frames_per_cell = 2;
    else                    nes_frames_per_cell = 1;


    float dt_level = nes_frames_per_cell * nes_frame_time;
    float dt_total = m_DtRemained + dt;

    int32_t harddrop_count = 0;
    while (dt_total > dt_level + 0.00000001) {
        harddrop_count += 1;
        dt_total -= dt_level;
    }

    m_DtRemained = dt_total;
    return harddrop_count;
}

