#include <games/tetris/Tetromino.hpp>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_timer.h>
#include <games/tetris/Tetris.hpp>
#include <imgui.h>

// Todo: change to new font scaling api in imgui first
// Todo: test text with hardcoded gap + dummy to ensure it gets placed as expected

Tetris::Tetris() :
    m_ActiveTetromino(m_Board),
    m_NextTetromino(m_Board)
{
    m_TetrominoCounters[(size_t)m_ActiveTetromino.GetId()] += 1;
}

bool Tetris::Update(std::vector<SDL_Event> &events, RenderGroup &render_group) {
    V3F32 clear_color = V3F32(0.2f, 0.2f, 0.2f);
    render_group.SetCameraSize(4.0f, 3.0f);
    render_group.Clear(clear_color);

    uint64_t milliseconds_since_t0 = SDL_GetTicks();
    uint64_t milliseconds_since_t0_last = m_MillisecondsSinceT0Last;
    uint64_t milliseconds_dt = milliseconds_since_t0 - milliseconds_since_t0_last;
    float seconds_dt = static_cast<float>(milliseconds_dt) / 1000.0f;
    m_MillisecondsSinceT0Last = milliseconds_since_t0;


    if (!m_Paused) {
        uint32_t drop_count = GetDropCount(seconds_dt);
        while (drop_count) {
            bool moved_down = m_ActiveTetromino.MaybeMoveDown();
            if (!moved_down) {
                HandleTetrominoPlacement();
            }
            drop_count--;
        }
    }


    for (auto &event : events) {
        if (m_Paused) {
            UpdatePaused(event);
        }
        else {
            UpdateRunning(event, seconds_dt);
        }
    }

    m_Board.Draw(m_Level, render_group);
    m_ActiveTetromino.Draw(render_group);

    DrawNextTetromino(render_group);
    DrawStatistics(render_group);
    DrawLineCounter(render_group);
    DrawLevel(render_group);
    DrawScore(render_group);
    if (m_Paused) {
        DrawPauseMenu(render_group);
    }

    return m_Running;
}

void Tetris::UpdateRunning(SDL_Event &event, float dt) {
    switch (event.type) {
    case SDL_EVENT_KEY_DOWN: {
        auto key = event.key.key;
        if (key == SDLK_RIGHT) {
            m_ActiveTetromino.MaybeMoveHorizontally(TetrominoDirection::Right);
        } else if (key == SDLK_LEFT) {
            m_ActiveTetromino.MaybeMoveHorizontally(TetrominoDirection::Left);
        } else if (key == SDLK_DOWN) {
            bool moved_down = m_ActiveTetromino.MaybeMoveDown();
            if (!moved_down) {
                HandleTetrominoPlacement();
            }
            else {
                m_SoftdropCounter++;
            }
        } else if (key == SDLK_X) {
            m_ActiveTetromino.MaybeRotate(TetrominoRotation::Clockwise);
        } else if (key == SDLK_Z || key == SDLK_Y) {
            m_ActiveTetromino.MaybeRotate(TetrominoRotation::CounterClockwise);
        } else if (key == SDLK_ESCAPE) {
            m_Paused = true;
        }
    }
    default:;
    }
}

void Tetris::UpdatePaused(SDL_Event &event) {
    switch (event.type) {
    case SDL_EVENT_KEY_DOWN: {
        auto key = event.key.key;
        if (key == SDLK_ESCAPE) {
            m_Paused = false;
        }
    }
    default:;
    }
}

void Tetris::HandleTetrominoPlacement() {
    int32_t rows_cleared = m_Board.PlaceTetromino(m_ActiveTetromino);

    m_ActiveTetromino = m_NextTetromino;
    m_NextTetromino = Tetromino(m_Board);

    m_LineCounter += rows_cleared;
    m_TetrominoCounters[(size_t)m_ActiveTetromino.GetId()] += 1;

    if (rows_cleared == 1) {
        m_Score += 40 * (m_Level + 1);
    }
    else if (rows_cleared == 2) {
        m_Score += 100 * (m_Level + 1);
    }
    else if (rows_cleared == 3) {
        m_Score += 300 * (m_Level + 1);
    }
    else if (rows_cleared == 4) {
        m_Score += 1200 * (m_Level + 1);
    }

    m_Score += m_SoftdropCounter;
    m_SoftdropCounter = 0;

    m_Level = m_StartingLevel + m_LineCounter / 10;
}

uint32_t Tetris::GetDropCount(float dt) {
    float nes_frame_time = 1.0f / 60;
    int32_t nes_frames_per_cell;
    if      (m_Level <= 8)  nes_frames_per_cell = 48 - m_Level * 5;
    else if (m_Level == 9)  nes_frames_per_cell = 6;
    else if (m_Level <= 12) nes_frames_per_cell = 5;
    else if (m_Level <= 15) nes_frames_per_cell = 4;
    else if (m_Level <= 18) nes_frames_per_cell = 3;
    else if (m_Level <= 28) nes_frames_per_cell = 2;
    else                    nes_frames_per_cell = 1;


    float dt_level = static_cast<float>(nes_frames_per_cell) * nes_frame_time;
    float dt_total = m_DtInSecondsRemaining + dt;

    uint32_t drop_count = 0;
    while (dt_total > dt_level + 0.00000001) {
        drop_count += 1;
        dt_total -= dt_level;
    }

    m_DtInSecondsRemaining = dt_total;
    return drop_count;
}

void Tetris::DrawPauseMenu(RenderGroup &render_group) {
    ImGui::Begin("TetrisPause");
    if (ImGui::Button("Resume")) {
        m_Paused = false;
    }
    if (ImGui::Button("Exit")) {
        m_Running = false;
    }
    ImGui::End();
}

void Tetris::DrawLineCounter(RenderGroup &render_group) {
    V2F32 view_pos = {0.5f, 2.6f};
    ImVec2 screen_pos = render_group.ViewPosToScreenPosImGui(view_pos);

    ImGui::SetNextWindowPos(screen_pos);
    ImGui::Begin("TetrisLines", nullptr, m_ImGuiWindowFlags);
    ImGui::Text("LINES - %d", m_LineCounter);
    ImGui::End();
}

void Tetris::DrawStatistics(RenderGroup &render_group) {
    V2F32 view_tetrominoes_pos = {0.4f, 1.8f};
    V2F32 view_advance = {0.0f, 0.2f};

    V2F32 view_text_title_pos = view_tetrominoes_pos + V2F32(0.02f, 0.4f);
    ImVec2 screen_text_title_pos = render_group.ViewPosToScreenPosImGui(view_text_title_pos);

    V2F32 view_text_pos = view_tetrominoes_pos + V2F32(0.4f, 0.16f);
    V2F32 view_text_gap = {0.0f, 0.124f};
    ImVec2 screen_text_pos = render_group.ViewPosToScreenPosImGui(view_text_pos);
    ImVec2 screen_text_gap = render_group.ViewSizeToScreenSizeImGui(view_text_gap);


    using enum TetrominoId;

    Tetromino::Draw(TETROMINO_T, 0, view_tetrominoes_pos, 0.5f, render_group);
    view_tetrominoes_pos.y -= view_advance.y;

    Tetromino::Draw(TETROMINO_J, 0, view_tetrominoes_pos, 0.5f, render_group);
    view_tetrominoes_pos.y -= view_advance.y;

    Tetromino::Draw(TETROMINO_Z, 0, view_tetrominoes_pos, 0.5f, render_group);
    view_tetrominoes_pos.y -= view_advance.y;

    Tetromino::Draw(TETROMINO_O, 0, view_tetrominoes_pos, 0.5f, render_group);
    view_tetrominoes_pos.y -= view_advance.y;

    Tetromino::Draw(TETROMINO_S, 0, view_tetrominoes_pos, 0.5f, render_group);
    view_tetrominoes_pos.y -= view_advance.y;

    Tetromino::Draw(TETROMINO_L, 0, view_tetrominoes_pos, 0.5f, render_group);
    view_tetrominoes_pos.y -= view_advance.y;

    Tetromino::Draw(TETROMINO_I, 0, view_tetrominoes_pos, 0.5f, render_group);
    view_tetrominoes_pos.y -= view_advance.y;


    ImGui::SetNextWindowPos(screen_text_title_pos);
    ImGui::Begin("TetrisStatisticsTitle", nullptr, m_ImGuiWindowFlags);
    ImGui::Text("STATISTICS");
    ImGui::End();


    ImGui::SetNextWindowPos(screen_text_pos);
    ImGui::Begin("TetrisStatistics", nullptr, m_ImGuiWindowFlags);

    ImGui::Text("%d", m_TetrominoCounters[(size_t)TETROMINO_T]);
    ImGui::Dummy(screen_text_gap);
    ImGui::Text("%d", m_TetrominoCounters[(size_t)TETROMINO_J]);
    ImGui::Dummy(screen_text_gap);
    ImGui::Text("%d", m_TetrominoCounters[(size_t)TETROMINO_Z]);
    ImGui::Dummy(screen_text_gap);
    ImGui::Text("%d", m_TetrominoCounters[(size_t)TETROMINO_O]);
    ImGui::Dummy(screen_text_gap);
    ImGui::Text("%d", m_TetrominoCounters[(size_t)TETROMINO_S]);
    ImGui::Dummy(screen_text_gap);
    ImGui::Text("%d", m_TetrominoCounters[(size_t)TETROMINO_L]);
    ImGui::Dummy(screen_text_gap);
    ImGui::Text("%d", m_TetrominoCounters[(size_t)TETROMINO_I]);
    ImGui::Dummy(screen_text_gap);

    ImGui::End();
}

void Tetris::DrawScore(RenderGroup &render_group) {
    V2F32 view_pos = {3.0f, 2.2f};
    ImVec2 screen_pos = render_group.ViewPosToScreenPosImGui(view_pos);

    ImGui::SetNextWindowPos(screen_pos);
    ImGui::Begin("TetrisScore", nullptr, m_ImGuiWindowFlags);
    ImGui::Text("Score");
    ImGui::Text("%d", m_Score);
    ImGui::End();
}

void Tetris::DrawNextTetromino(RenderGroup &render_group) {
    V2F32 text_view_pos = {3.0f, 1.8f};
    ImVec2 text_screen_pos = render_group.ViewPosToScreenPosImGui(text_view_pos);

    ImGui::SetNextWindowPos(text_screen_pos);
    ImGui::Begin("TetrisNextTetromino", nullptr, m_ImGuiWindowFlags);
    ImGui::Text("Next");
    ImGui::End();


    V2F32 tetromino_view_pos = {3.0, 1.4f};
    Tetromino::Draw(m_NextTetromino.GetId(), 0, tetromino_view_pos, 0.5f, render_group);
}

void Tetris::DrawLevel(RenderGroup &render_group) {
    V2F32 view_pos = {3.0f, 1.2f};
    ImVec2 screen_pos = render_group.ViewPosToScreenPosImGui(view_pos);

    ImGui::SetNextWindowPos(screen_pos);
    ImGui::Begin("TetrisLevel", nullptr, m_ImGuiWindowFlags);
    ImGui::Text("Level");
    ImGui::Text("%d", m_Level);
    ImGui::End();
}

