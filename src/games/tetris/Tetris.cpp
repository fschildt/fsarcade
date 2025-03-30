#include <SDL3/SDL_events.h>
#include <SDL3/SDL_timer.h>
#include <games/tetris/Tetris.hpp>

void Tetris::Init() {
    m_Level = 0;
    m_DtRemaining = 0;
    m_TLast = SDL_GetTicks();

    m_Board.Init();
    m_ActiveTetromino.Init();
    m_NextTetromino.Init();
}

bool Tetris::Update(SDL_Window *window, RenderGroup& render_group) {
    uint64_t tnow = SDL_GetTicks(); // number of milliseconds since SDL3 was initialized
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
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_EVENT_QUIT:
        case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
        case SDL_EVENT_WINDOW_DESTROYED: {
             return false;
        }
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
    return true;
}

void Tetris::HandleTetrominoPlacement() {
    m_Board.PlaceTetromino(m_ActiveTetromino);
    int32_t rows_cleared = m_Board.ClearRows(m_ActiveTetromino.m_Y);
    m_ActiveTetromino = m_NextTetromino;
    m_NextTetromino.Init();
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

