#include <games/tetris/Tetromino.hpp>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_timer.h>
#include <games/tetris/Tetris.hpp>
#include <imgui.h>

#include <cstdio>
#include <cstring>
#include <iterator>

Tetris::Tetris() {
    m_TetrominoCounters[m_ActiveTetromino.m_Id] += 1;
}

bool Tetris::Update(std::vector<SDL_Event> &events, RenderGroup &render_group) {
    V3F32 clear_color = V3F32(0.2f, 0.2f, 0.2f);
    render_group.SetCameraSize(4.0f, 3.0f);
    render_group.Clear(clear_color);

    uint64_t tnow = SDL_GetTicks();
    float dt = (tnow - m_TLast) / 1000.f;
    m_TLast = tnow;


    if (!m_Paused) {
        uint32_t drop_count = GetDropCount(dt);
        while (drop_count) {
            bool moved_down = m_ActiveTetromino.MoveDown(m_Board.m_Bitmap);
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
            UpdateRunning(event, dt);
        }
    }

    m_Board.Draw(m_Level, render_group);
    m_ActiveTetromino.Draw(m_Level, render_group);
    DoImGui(render_group);
    DrawStatistics(render_group);

    return m_Running;
}

void Tetris::UpdateRunning(SDL_Event &event, float dt) {
    uint16_t *board_bitmap = m_Board.m_Bitmap;
    switch (event.type) {
    case SDL_EVENT_KEY_DOWN: {
        auto key = event.key.key;
        if (key == SDLK_RIGHT) {
            m_ActiveTetromino.MoveHorizontally(1, board_bitmap);
        } else if (key == SDLK_LEFT) {
            m_ActiveTetromino.MoveHorizontally(-1, board_bitmap);
        } else if (key == SDLK_DOWN) {
            bool moved_down = m_ActiveTetromino.MoveDown(board_bitmap);
            if (!moved_down) {
                HandleTetrominoPlacement();
            }
            else {
                m_SoftdropCounter++;
            }
        } else if (key == SDLK_X) {
            m_ActiveTetromino.Rotate(1, board_bitmap);
        } else if (key == SDLK_Z || key == SDLK_Y) {
            m_ActiveTetromino.Rotate(3, board_bitmap);
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
    m_Board.PlaceTetromino(m_ActiveTetromino);
    int32_t rows_cleared = m_Board.ClearRows(m_ActiveTetromino.m_Y);

    m_ActiveTetromino = m_NextTetromino;
    m_NextTetromino = Tetromino();

    m_LineCounter += rows_cleared;
    m_TetrominoCounters[m_ActiveTetromino.m_Id] += 1;

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

int32_t Tetris::GetDropCount(float dt) {
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
    float dt_total = m_DtRemaining + dt;

    int32_t drop_count = 0;
    while (dt_total > dt_level + 0.00000001) {
        drop_count += 1;
        dt_total -= dt_level;
    }

    m_DtRemaining = dt_total;
    return drop_count;
}

void Tetris::DoImGui(RenderGroup &render_group) {
    ImGuiIO& io = ImGui::GetIO();
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoInputs;

    float scale = render_group.GetScale() * 0.01f;
    ImVec2 pos_lines          = ImVec2(  0.0f * scale, 100.0f * scale);
    ImVec2 pos_score          = ImVec2(275.0f * scale, 100.0f * scale);
    ImVec2 pos_next_tetromino = ImVec2(275.0f * scale, 150.0f * scale);
    ImVec2 pos_level          = ImVec2(275.0f * scale, 200.0f * scale);


    uint32_t lines = m_LineCounter > 999999 ? 999999 : m_LineCounter;
    char lines_cstr[] = "Lines:      0";
    size_t r = strlen(lines_cstr) - 1;
    while (lines) {
        lines_cstr[r] = lines % 10 + '0';
        lines = lines / 10;
        r--;
    }
    ImGui::SetNextWindowPos(pos_lines);
    ImGui::Begin("TetrisLines", nullptr, flags);
    ImGui::Text("%s", lines_cstr);
    ImGui::End();


    uint32_t score = m_Score > 999999 ? 999999 : m_Score;
    char score_cstr[] = "Score: 000000";
    r = strlen(score_cstr) - 1;
    while (score) {
        score_cstr[r] = score % 10 + '0';
        score = score / 10;
        r--;
    }
    ImGui::SetNextWindowPos(pos_score);
    ImGui::Begin("TetrisScore", nullptr, flags);
    ImGui::Text("%s", score_cstr);
    ImGui::End();

    ImGui::SetNextWindowPos(pos_next_tetromino);
    ImGui::Begin("TetrisNextTetromino", nullptr, flags);
    ImGui::Text("Next: %d", m_NextTetromino.m_Id);
    ImGui::End();


    uint32_t level = m_Level > 99 ? 99 : m_Level;
    char level_cstr[] = "Level:  0";
    r = strlen(level_cstr) - 1;
    while (level) {
        level_cstr[r] = level % 10 + '0';
        level = level / 10;
        r--;
    }
    ImGui::SetNextWindowPos(pos_level);
    ImGui::Begin("TetrisLevel", nullptr, flags);
    ImGui::Text("%s", level_cstr);
    ImGui::End();


    if (m_Paused) {
        ImGui::Begin("TetrisPause");
        if (ImGui::Button("Resume")) {
            m_Paused = false;
        }
        if (ImGui::Button("Exit")) {
            m_Running = false;
        }
        ImGui::End();
    }
}

void Tetris::DrawStatistics(RenderGroup &render_group) {
    ImGuiIO& io = ImGui::GetIO();
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoScrollbar;
//    flags |= ImGuiWindowFlags_NoBackground;

    // Todo: change to new font scaling api in imgui first

    float scale = render_group.GetScale();
    float screen_text_scale = 2.0f * scale / 256.0f; // 256 = 1024/4 and 768/3

    V2F32 view_tetrominoes_pos = {0.4f, 1.6f};
    V2F32 view_text_pos = view_tetrominoes_pos + V2F32(0.4f, 0.16f);
    V2F32 view_advance = {0.0f, 0.2f};
    ImVec2 screen_text_pos = render_group.ViewPosToImguiPos(view_text_pos);


    Tetromino::Draw(view_tetrominoes_pos, TETROMINO_T, 0, 0, 0.5f, render_group);
    view_tetrominoes_pos.y -= view_advance.y;

    Tetromino::Draw(view_tetrominoes_pos, TETROMINO_J, 0, 0, 0.5f, render_group);
    view_tetrominoes_pos.y -= view_advance.y;

    Tetromino::Draw(view_tetrominoes_pos, TETROMINO_Z, 0, 0, 0.5f, render_group);
    view_tetrominoes_pos.y -= view_advance.y;

    Tetromino::Draw(view_tetrominoes_pos, TETROMINO_O, 0, 0, 0.5f, render_group);
    view_tetrominoes_pos.y -= view_advance.y;

    Tetromino::Draw(view_tetrominoes_pos, TETROMINO_S, 0, 0, 0.5f, render_group);
    view_tetrominoes_pos.y -= view_advance.y;

    Tetromino::Draw(view_tetrominoes_pos, TETROMINO_L, 0, 0, 0.5f, render_group);
    view_tetrominoes_pos.y -= view_advance.y;

    Tetromino::Draw(view_tetrominoes_pos, TETROMINO_I, 0, 0, 0.5f, render_group);
    view_tetrominoes_pos.y -= view_advance.y;


    ImGui::SetNextWindowPos(screen_text_pos);
    ImGui::Begin("TetrisStatistics", nullptr, flags);
    ImGui::SetWindowFontScale(screen_text_scale);

    float screen_text_line_height = ImGui::GetTextLineHeight();
    ImVec2 screen_gap = render_group.ViewDimToImguiDim(view_advance);
    ImVec2 screen_text_gap = {
        screen_gap.x,
        screen_gap.y - screen_text_line_height
    };

    ImGui::Text("%d", m_TetrominoCounters[TETROMINO_T]);
    ImGui::Dummy(screen_text_gap);
    ImGui::Text("%d", m_TetrominoCounters[TETROMINO_J]);
    ImGui::Dummy(screen_text_gap);
    ImGui::Text("%d", m_TetrominoCounters[TETROMINO_Z]);
    ImGui::Dummy(screen_text_gap);
    ImGui::Text("%d", m_TetrominoCounters[TETROMINO_O]);
    ImGui::Dummy(screen_text_gap);
    ImGui::Text("%d", m_TetrominoCounters[TETROMINO_S]);
    ImGui::Dummy(screen_text_gap);
    ImGui::Text("%d", m_TetrominoCounters[TETROMINO_L]);
    ImGui::Dummy(screen_text_gap);
    ImGui::Text("%d", m_TetrominoCounters[TETROMINO_I]);
    ImGui::Dummy(screen_text_gap);

    ImGui::End();
}

