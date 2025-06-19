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


    m_UncoveredPositions.reserve(MAX_MAP_WIDTH * MAX_MAP_HEIGHT);
    m_CoveredPositions.reserve((size_t)(1.3f * MAX_MAP_WIDTH * MAX_MAP_HEIGHT));

    for (int32_t y = 0; y < m_MapHeight; y++) {
        for (int32_t x = 0; x < m_MapHeight; x++) {
            m_CoveredPositions.emplace(x, y);
        }
    }


    int32_t mine_count = 40;
    assert(m_MapWidth == 16 && m_MapHeight == 16);

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

bool Minesweeper::Update(std::vector<SDL_Event> &events, RenderGroup &render_group) {
    V3F32 clear_color = {0.3f, 0.2f, 0.3f};
    render_group.SetCameraSize(4.0f, 3.0f);
    render_group.Clear(clear_color);

    for (SDL_Event &event : events) {
        if (!m_IsRunning) {
            return false;
        }
        if (m_IsPaused) {
            ProcessEventDuringPause(event, render_group);
        }
        else {
            ProcessEventDuringResume(event, render_group);
        }
    }

    if (m_IsPaused) {
        DrawPauseMenu(render_group);
    }

    DrawBoard(render_group);

    return m_IsRunning;
}

void Minesweeper::ProcessEventDuringPause(SDL_Event &event, RenderGroup &render_group) {
    switch (event.type) {
    case SDL_EVENT_KEY_DOWN: {
        if (event.key.key == SDLK_ESCAPE) {
            m_IsPaused = false;
        }
    }
    default:;
    }
}

void Minesweeper::ProcessEventDuringResume(SDL_Event &event, RenderGroup &render_group) {
    switch (event.type) {
    case SDL_EVENT_KEY_DOWN: {
        if (event.key.key == SDLK_ESCAPE) {
            m_IsPaused = true;
        }
    } break;

    case SDL_EVENT_MOUSE_BUTTON_DOWN: {
        V2F32 click_screen_pos = {event.button.x, (float)render_group.m_ScreenHeight -1 - event.button.y};
        V2F32 click_view_pos = ScreenPosToViewPos(click_screen_pos, render_group);

        int32_t x = (int32_t)((click_view_pos.x - m_MapViewPos.x) / m_CellOuterViewSize.x);
        int32_t y = (int32_t)((click_view_pos.y - m_MapViewPos.y) / m_CellOuterViewSize.y);
        if (x < 0 || x >= m_MapWidth) {
            break;
        }
        if (y < 0 || y >= m_MapHeight) {
            break;
        }

        if (event.button.button == 1) {
            if (IsCovered(x, y)) {
                Uncover(x, y);
            }
        }
        else if (event.button.button == 3) {
            if (IsCovered(x, y)) {
                ToggleFlag(x ,y);
                if (IsFlagged(x, y)) {
                    m_FlaggedPositions.insert(V2ST(x, y));
                }
                else {
                    m_FlaggedPositions.erase(V2ST(x, y));
                }
            }
        }

    } break;

    default:;
    }
}

void Minesweeper::Uncover(int32_t x, int32_t y) {
    m_IsCoveredBitmap[y] &= ~(1 << x);
    m_CoveredPositions.erase(V2ST(x, y));
    m_UncoveredPositions.emplace_back(x, y);
    if (IsMine(x, y)) {
        m_IsRunning = false;
    }
    if (IsFlagged(x, y)) {
        ToggleFlag(x, y);
        if (IsFlagged(x, y)) {
            m_FlaggedPositions.insert(V2ST(x, y));
        }
        else {
            m_FlaggedPositions.erase(V2ST(x, y));
        }
    }
    CountAdjacentMines(x, y);
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

int32_t Minesweeper::CountAdjacentMines(int32_t x, int32_t y) {
    assert(!IsMine(x, y));

    int32_t count = 0;
    int32_t x0 = x - 1 >= 0 ? x-1 : 0;
    int32_t x1 = x + 1 < m_MapWidth ? x + 1 : x;
    int32_t y0 = y - 1 >= 0 ? y-1 : 0;
    int32_t y1 = y + 1 < m_MapHeight ? y + 1 : y;
    for (int32_t y = y0; y <= y1; y++) {
        for (int32_t x = x0; x <= x1; x++) {
            if (m_IsMineBitmap[y] & 1 << x) {
                count++;
            }
        }
    }
    printf("count = %d\n", count);
    return count;
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

void Minesweeper::DrawPauseMenu(RenderGroup &render_group) {
    ImGui::Begin("TetrisPause");
    if (ImGui::Button("Resume")) {
        m_IsPaused = false;
    }
    if (ImGui::Button("Exit")) {
        m_IsRunning = false;
    }
    ImGui::End();
}

void Minesweeper::DrawBoard(RenderGroup &render_group) {
    V3F32 covered_cell_color = V3F32(0.4f, 0.4f, 0.4f);
    V3F32 uncovered_cell_color = V3F32(0.2f, 0.2f, 0.2f);

    for (V2ST pos : m_CoveredPositions) {
        V3F32 world_pos = {
            m_MapViewPos.x + (float)pos.x * m_CellOuterViewSize.x,
            m_MapViewPos.y + (float)pos.y * m_CellOuterViewSize.y,
            0.0f
        };
        render_group.PushRectangle(world_pos, m_CellInnerViewSize, covered_cell_color);
    }
    for (V2ST pos: m_FlaggedPositions) {
        V2F32 draw_size = {m_CellInnerViewSize.x * 0.5f, m_CellInnerViewSize.y * 0.5f};
        V2F32 draw_offset = {
            (m_CellInnerViewSize.x - draw_size.x) / 2,
            (m_CellInnerViewSize.y - draw_size.y) / 2
        };
        V3F32 world_pos = {
            m_MapViewPos.x + (float)pos.x * m_CellOuterViewSize.x + draw_offset.x,
            m_MapViewPos.y + (float)pos.y * m_CellOuterViewSize.y + draw_offset.y,
            1.0f
        };
        V3F32 flag_color = {0.6f, 0.3f, 03.f};
        render_group.PushRectangle(world_pos, draw_size, flag_color);
    }
    for (V2ST pos : m_UncoveredPositions) {
        V3F32 world_pos = {
            m_MapViewPos.x + (float)pos.x * m_CellOuterViewSize.x,
            m_MapViewPos.y + (float)pos.y * m_CellOuterViewSize.y,
            0.0f
        };
        render_group.PushRectangle(world_pos, m_CellInnerViewSize, uncovered_cell_color);
    }
}

