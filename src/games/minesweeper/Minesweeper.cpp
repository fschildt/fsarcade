#include <algorithm>
#include <asm-generic/errno.h>
#include <renderer/RenderGroup.hpp>
#include <games/minesweeper/Minesweeper.hpp>
#include <random>

// Todo: count adjacent mines only in the beginning?
// Todo: draw a difficulty selection menu at top-mid.

// Note: How many mines?
// - Beginner: 8x8 or 9x9 grid with 10 mines.
// - Intermediate: 16x16 grid with 40 mines.
// - Expert: 30x16 grid with 99 mines.

Minesweeper::Minesweeper() {
    float map_width = static_cast<float>(m_MapWidth);
    float map_height = static_cast<float>(m_MapHeight);
    float cell_size = 0.8f * std::min(m_WorldHeight / MAX_MAP_HEIGHT, m_WorldWidth / MAX_MAP_WIDTH);
    float cell_size_without_border = 0.8f * cell_size;

    m_MapViewPos = {
        0.1f * cell_size_without_border + (m_WorldWidth - cell_size * map_width) / 2,
        0.1f * cell_size_without_border + (m_WorldHeight - cell_size * map_height) / 2
    };
    m_CellOuterViewSize = {cell_size, cell_size};
    m_CellInnerViewSize = {cell_size_without_border, cell_size_without_border};

    // Todo: assert various stuff

    Reinit();
}

void Minesweeper::Reinit() {
    int32_t mine_count = 40;
    memset(m_IsCoveredBitmap, 0xff, sizeof(m_IsCoveredBitmap));
    memset(m_IsFlaggedBitmap, 0 , sizeof(m_IsFlaggedBitmap));
    InitIsMineBitmap(mine_count);
    InitAdjacentMineCounters();
}

void Minesweeper::InitIsMineBitmap(int32_t mine_count) {
    assert(mine_count < m_MapWidth * m_MapHeight);

    memset(m_IsMineBitmap, 0 , sizeof(m_IsMineBitmap));

    std::mt19937 rng((std::random_device()()));
    std::uniform_int_distribution<int32_t> dist(0, m_MapWidth * m_MapHeight - 1);

    while (mine_count) {
        int32_t random_pos = dist(rng);
        int32_t x = random_pos / m_MapWidth;
        int32_t y = random_pos % m_MapWidth;
        if (!IsMine(x, y)) {
            m_IsMineBitmap[y] |= 1 << x;
            mine_count--;
        }
    }
}

void Minesweeper::InitAdjacentMineCounters() {
    for (int32_t y = 0; y < m_MapHeight; y++) {
        int32_t y0 = y > 0 ? y-1 : y;
        int32_t y1 = y < m_MapHeight-1 ? y+1 : y;

        for (int32_t x = 0; x < m_MapHeight; x++) {
            int32_t x0 = x > 0 ? x-1 : x;
            int32_t x1 = x < m_MapWidth-1 ? x+1 : x;

            int32_t adjacent_mine_counter = 0;
            for (int32_t inner_y = y0; inner_y <= y1; inner_y++) {
                for (int32_t inner_x = x0; inner_x <= x1; inner_x++) {
                    if (IsMine(inner_y, inner_x)) {
                        adjacent_mine_counter++;
                    }
                }
            }

            m_AdjacentMineCounters[y * m_MapWidth + x] = adjacent_mine_counter;
        }
    }
}

bool Minesweeper::Update(std::vector<SDL_Event> &events, RenderGroup &render_group) {
    V3F32 clear_color = {0.3f, 0.2f, 0.3f};
    render_group.SetCameraSize(4.0f, 3.0f);
    render_group.Clear(clear_color);

    if (m_RunState == MinesweeperRunState::Restart) {
        Reinit();
        m_RunState = MinesweeperRunState::Resume;
    }

    for (SDL_Event &event : events) {
        if (m_RunState == MinesweeperRunState::Exit) {
            return false;
        }
        else if (m_RunState == MinesweeperRunState::Pause) {
            m_RunState = ProcessEventDuringPause(event, render_group);
        }
        else if (m_RunState == MinesweeperRunState::Resume) {
            m_RunState = ProcessEventDuringResume(event, render_group);
        }
    }

    if (m_RunState == MinesweeperRunState::Pause) {
        m_RunState = DrawPauseMenu(render_group);
    }
    else if (m_RunState == MinesweeperRunState::GameOver) {
        m_RunState = DrawGameOverMenu(render_group);
    }

    DrawBoard(render_group);

    bool keep_running = m_RunState != MinesweeperRunState::Exit;
    return keep_running;
}

MinesweeperRunState Minesweeper::ProcessEventDuringPause(SDL_Event &event, RenderGroup &render_group) {
    MinesweeperRunState run_state = MinesweeperRunState::Pause;

    switch (event.type) {
    case SDL_EVENT_KEY_DOWN: {
        if (event.key.key == SDLK_ESCAPE) {
            run_state = MinesweeperRunState::Resume;
        }
    } break;
    default:;
    }

    return run_state;
}

MinesweeperRunState Minesweeper::ProcessEventDuringResume(SDL_Event &event, RenderGroup &render_group) {
    MinesweeperRunState run_state = MinesweeperRunState::Resume;

    switch (event.type) {
    case SDL_EVENT_KEY_DOWN: {
        if (event.key.key == SDLK_ESCAPE) {
            run_state = MinesweeperRunState::Pause;
        }
    } break;

    case SDL_EVENT_MOUSE_BUTTON_DOWN: {
        V2F32 click_screen_pos = {event.button.x, (float)render_group.m_ScreenHeight -1 - event.button.y};
        V2F32 click_view_pos = ScreenPosToViewPos(click_screen_pos, render_group);

        float x_adjusted = click_view_pos.x - m_MapViewPos.x;
        float y_adjusted = click_view_pos.y - m_MapViewPos.y;
        if (x_adjusted < 0.0f) {
            break;
        }
        if (y_adjusted < 0.0f) {
            break;
        }

        int32_t x = (int32_t)(x_adjusted / m_CellOuterViewSize.x);
        int32_t y = (int32_t)(y_adjusted / m_CellOuterViewSize.y);
        if (x >= m_MapWidth) {
            break;
        }
        if (y >= m_MapHeight) {
            break;
        }

        if (event.button.button == 1) {
            if (IsCovered(x, y)) {
                Uncover(x, y);
                if (IsMine(x, y)) {
                    run_state = MinesweeperRunState::GameOver;
                }
            }
        }
        else if (event.button.button == 3) {
            if (IsCovered(x, y)) {
                ToggleFlag(x ,y);
            }
        }

    } break;

    default:;
    }

    return run_state;
}

void Minesweeper::Uncover(int32_t x, int32_t y) {
    m_IsCoveredBitmap[y] &= ~(1 << x);
    if (IsMine(x, y)) {
        m_RunState = MinesweeperRunState::GameOver;
    }
    if (IsFlagged(x, y)) {
        ToggleFlag(x, y);
    }
}

void Minesweeper::ToggleFlag(int32_t x, int32_t y) {
    m_IsFlaggedBitmap[y] ^= (1 << x);
}

bool Minesweeper::IsCovered(int32_t x, int32_t y) {
    bool is_covered = m_IsCoveredBitmap[y] & 1 << x;
    return is_covered;
}

bool Minesweeper::IsFlagged(int32_t x, int32_t y) {
    bool is_flagged = m_IsFlaggedBitmap[y] & 1 << x;
    return is_flagged;
}

bool Minesweeper::IsMine(int32_t x, int32_t y) {
    bool is_mine = m_IsMineBitmap[y] & 1 << x;
    return is_mine;
}

V2F32 Minesweeper::ScreenPosToViewPos(V2F32 screen_pos, RenderGroup &render_group) {
    // e.g. [0, 1024] -> [0, 1] -> [0, 4]
    // e.g. [0,  768] -> [0, 1] -> [0, 3]
    float screen_width = (float)render_group.m_ScreenWidth;
    float screen_height = (float)render_group.m_ScreenHeight;

    V2F32 view_pos;
    view_pos.x = (screen_pos.x / screen_width) * m_WorldWidth;
    view_pos.y = (screen_pos.y / screen_height) * m_WorldHeight;
    return view_pos;
}

MinesweeperRunState Minesweeper::DrawPauseMenu(RenderGroup &render_group) {
    MinesweeperRunState run_state = m_RunState;

    ImGui::Begin("MinesweeperPause");
    if (ImGui::Button("Resume")) {
        run_state = MinesweeperRunState::Resume;
    }
    if (ImGui::Button("Exit")) {
        run_state = MinesweeperRunState::Exit;
    }
    ImGui::End();

    return run_state;
}

MinesweeperRunState Minesweeper::DrawGameOverMenu(RenderGroup &render_group) {
    MinesweeperRunState run_state = m_RunState;

    ImGui::Begin("MinesweeperGameOver");
    ImGui::Text("Score = ???");
    if (ImGui::Button("Restart")) {
        run_state = MinesweeperRunState::Restart;
    }
    if (ImGui::Button("Exit")) {
        run_state = MinesweeperRunState::Exit;
    }
    ImGui::End();

    return run_state;
}

void Minesweeper::DrawBoard(RenderGroup &render_group) {
    V3F32 covered_cell_color = V3F32(0.4f, 0.4f, 0.4f);
    V3F32 uncovered_cell_color = V3F32(0.2f, 0.2f, 0.2f);
    V3F32 flag_color = {0.6f, 0.3f, 03.f};
    V3F32 mine_color = {0.8f, 0.2f, 0.2f};

    V2F32 flag_draw_size = {m_CellInnerViewSize.x * 0.5f, m_CellInnerViewSize.y * 0.5f};
    V2F32 flag_draw_offset = {
        (m_CellInnerViewSize.x - flag_draw_size.x) / 2,
        (m_CellInnerViewSize.y - flag_draw_size.y) / 2
    };



    // Todo: avoid if-statement by having them in separate contiguous locations?

    for (int32_t y = 0; y < m_MapHeight; y++) {
        for (int32_t x = 0; x < m_MapWidth; x++) {
            V3F32 world_pos = {
                m_MapViewPos.x + (float)x * m_CellOuterViewSize.x,
                m_MapViewPos.y + (float)y * m_CellOuterViewSize.y,
                0.0f
            };
            bool is_covered = IsCovered(x, y);
            bool is_flagged = IsFlagged(x, y);
            bool is_mine = IsMine(x, y);

            if (is_covered) {
                render_group.PushRectangle(world_pos, m_CellInnerViewSize, covered_cell_color);
            }
            else {
                render_group.PushRectangle(world_pos, m_CellInnerViewSize, uncovered_cell_color);
            }
            if (is_flagged) {
                assert(IsCovered(x ,y));
                V3F32 flag_world_pos = {
                    world_pos.x + flag_draw_offset.x,
                    world_pos.y + flag_draw_offset.y,
                    1.0f
                };
                render_group.PushRectangle(flag_world_pos, flag_draw_size, flag_color);
            }
            if (!is_covered && is_mine) {
                V3F32 mine_world_pos = {
                    world_pos.x,
                    world_pos.y,
                    2.0f
                };
                render_group.PushRectangle(mine_world_pos, m_CellInnerViewSize, mine_color);
            }
        }
    }
}

