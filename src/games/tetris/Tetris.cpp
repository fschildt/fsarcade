#include <games/tetris/Tetromino.hpp>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_timer.h>
#include <games/tetris/Tetris.hpp>
#include <imgui.h>

#include <cstdio>
#include <cstring>

Tetris::Tetris() {
    Tetromino::InitRng();
    m_TetrominoCounters[m_ActiveTetromino.m_Id] += 1;
}

void Tetris::Init() {
}

bool Tetris::Update(std::vector<SDL_Event> &events, RenderGroup &render_group) {
    uint64_t tnow = SDL_GetTicks();
    float dt = (tnow - m_TLast) / 1000.f;
    m_TLast = tnow;


    V3 clear_color = V3(0.f, 0.f, 0.f);
    render_group.SetSize(10, 20);
    render_group.PushClear(clear_color);


    uint32_t drop_count = GetDropCount(dt);
    while (drop_count) {
        bool moved_down = m_ActiveTetromino.MoveDown(m_Board.m_Bitmap);
        if (!moved_down) {
            HandleTetrominoPlacement();
        }
        drop_count--;
    }


    uint16_t *board_bitmap = m_Board.m_Bitmap;
    for (SDL_Event &event : events) {
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
            } else if (key == SDLK_X) {
                m_ActiveTetromino.Rotate(1, board_bitmap);
            } else if (key == SDLK_Z) {
                m_ActiveTetromino.Rotate(3, board_bitmap);
            }
        }
        default:;
        }
    }

    m_Board.Draw(m_Level, render_group);
    m_ActiveTetromino.Draw(m_Level, render_group);
    DoImGui();
    return true;
}

void Tetris::HandleTetrominoPlacement() {
    m_Board.PlaceTetromino(m_ActiveTetromino);
    int32_t rows_cleared = m_Board.ClearRows(m_ActiveTetromino.m_Y);

    m_ActiveTetromino = m_NextTetromino;
    m_NextTetromino = Tetromino();

    m_LineCounter += rows_cleared;
    m_TetrominoCounters[m_ActiveTetromino.m_Id] += 1;

    // Todo: soft drop score
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

// Todo: can you change ImGui::Text() calls without formatting the strings like that?
void Tetris::DoImGui() {
    ImGuiIO& io = ImGui::GetIO(); (void)io;


    ImGuiWindowFlags flags = ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;


    ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x - 200, 0));
    ImGui::Begin("Fps", nullptr, flags);
    ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
    ImGui::End();


    uint32_t lines = m_LineCounter > 999999 ? 999999 : m_LineCounter;
    char lines_cstr[] = "Lines:      0";
    size_t r = strlen(lines_cstr) - 1;
    while (lines) {
        lines_cstr[r] = lines % 10 + '0';
        lines = lines / 10;
        r--;
    }
    ImGui::SetNextWindowPos(ImVec2(160, 50));
    ImGui::Begin("Lines", nullptr, flags);
    ImGui::Text("%s", lines_cstr);
    ImGui::End();


    ImGui::SetNextWindowPos(ImVec2(0, 100));
    ImGui::Begin("Statistics", nullptr, flags);
    ImGui::Text("O Piece = %d", m_TetrominoCounters[TETROMINO_O]);
    ImGui::Text("S Piece = %d", m_TetrominoCounters[TETROMINO_S]);
    ImGui::Text("Z Piece = %d", m_TetrominoCounters[TETROMINO_Z]);
    ImGui::Text("T Piece = %d", m_TetrominoCounters[TETROMINO_T]);
    ImGui::Text("L Piece = %d", m_TetrominoCounters[TETROMINO_L]);
    ImGui::Text("J Piece = %d", m_TetrominoCounters[TETROMINO_J]);
    ImGui::Text("I Piece = %d", m_TetrominoCounters[TETROMINO_I]);
    ImGui::End();


    uint32_t score = m_Score > 999999 ? 999999 : m_Score;
    char score_cstr[] = "Score: 000000";
    r = strlen(score_cstr) - 1;
    while (score) {
        score_cstr[r] = score % 10 + '0';
        score = score / 10;
        r--;
    }
    ImGui::SetNextWindowPos(ImVec2(250.f, 100.f));
    ImGui::Begin("Score", nullptr, flags);
    ImGui::Text("%s", score_cstr);
    ImGui::End();

    ImGui::SetNextWindowPos(ImVec2(250.f, 150.f));
    ImGui::Begin("NextTetromino", nullptr, flags);
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
    ImGui::SetNextWindowPos(ImVec2(250.f, 200.f));
    ImGui::Begin("Level", nullptr, flags);
    ImGui::Text("%s", level_cstr);
    ImGui::End();
}

