#include <SDL3/SDL_events.h>
#include <SDL3/SDL_timer.h>
#include <games/snake/Snake.hpp>
#include <imgui.h>
#include <random>


Snake::Snake ()
    : m_Rng(std::mt19937((std::random_device()()))) {
}

void Snake::Init() {
    size_t head_x = m_Width / 2;
    size_t head_y = m_Height / 2;
    m_BodyPositions[0] = V2ST(head_x -1, head_y);
    m_BodyPositions[1] = V2ST(head_x, head_y);
    m_BodyBitmap[head_y] = 3 << (m_Width - 1 - head_x);

    PlaceFood();
    m_LastMillisecondsSinceT0 = SDL_GetTicks();

    m_IsInitialized = true;
}

bool Snake::Update(std::vector<SDL_Event> &events, RenderGroup &render_group) {
    if (!m_IsInitialized) {
        Init();
    }

    uint64_t milliseconds_since_t0 = SDL_GetTicks();
    float dt_in_seconds = (milliseconds_since_t0 - m_LastMillisecondsSinceT0) / 1000.0f;
    m_LastMillisecondsSinceT0 = milliseconds_since_t0;


    V3F32 clear_color = V3F32(0.0f, 0.0f, 0.0f);
    render_group.SetSize(10.0f, 20.0f);
    render_group.PushClear(clear_color);


    if (!m_IsPaused) {
        MaybeMoveSnake(dt_in_seconds);
    }

    for (SDL_Event &event : events) {
        if (!m_IsRunning) {
            printf("event loop is running just false\n");
            return false;
        }
        if (m_IsPaused) {
            ProcessEventDuringPause(event);
        }
        else {
            ProcessEventDuringResume(event);
        }
    }

    Draw(render_group);
    DoImgui();

    return m_IsRunning;
}

void Snake::MaybeMoveSnake(float dt_in_seconds) {
    float dt_in_seconds_to_use = m_DtInSecondsRemaining + dt_in_seconds;
    float tiles_per_second = m_TilesPerSecond;
    float seconds_per_tile = 1.0f / tiles_per_second;
    while (dt_in_seconds_to_use > seconds_per_tile + 0.00000001) {
        V2ST head_pos = m_BodyPositions[m_Head];
        V2ST tail_pos = m_BodyPositions[m_Tail];


        // find head_pos
        if (m_Direction == DIRECTION_UP) {
            head_pos.y += 1;
        }
        else if (m_Direction == DIRECTION_DOWN) {
            head_pos.y -= 1;
        }
        else if (m_Direction == DIRECTION_RIGHT) {
            head_pos.x += 1;
        }
        else if (m_Direction == DIRECTION_LEFT) {
            head_pos.x -= 1;
        }
        if (head_pos.y >= m_Height || head_pos.x >= m_Width) {
            m_IsRunning = false;
            return;
        }
        uint64_t head_bit = 1 << head_pos.x;
        uint64_t body_bits = m_BodyBitmap[head_pos.y];
        if (head_pos.y == tail_pos.y) {
            body_bits &= ~(1 << tail_pos.x);
        }
        if (head_bit & body_bits) {
            m_IsRunning = false;
            return;
        }


        // add head_pos
        size_t max_positions = sizeof(m_BodyPositions) / sizeof(m_BodyPositions[0]);
        m_Head += 1;
        if (m_Head >= max_positions) {
            m_Head = 0;
        }
        m_BodyPositions[m_Head] = head_pos;
        m_BodyBitmap[head_pos.y] |= (1 << head_pos.x);


        if (m_BodyPositions[m_Head] == m_FoodPosition) {
            PlaceFood();
        }
        else {
            // delete tail from bitmap
            V2ST tail_pos = m_BodyPositions[m_Tail];
            m_BodyBitmap[tail_pos.y] &= ~(1 << tail_pos.x);

            // move tail to next pos
            m_Tail += 1;
            if (m_Tail >= max_positions) {
                m_Tail = 0;
            }
        }

        m_LastAdvancedDirection = m_Direction;
        dt_in_seconds_to_use -= seconds_per_tile;
    }


    m_DtInSecondsRemaining = dt_in_seconds_to_use;
}

void Snake::ProcessEventDuringPause(SDL_Event &event) {
    switch (event.type) {
    case SDL_EVENT_KEY_DOWN: {
        if (event.key.key == SDLK_ESCAPE) {
            m_IsPaused = false;
        }
    }
    default:;
    }
}

void Snake::ProcessEventDuringResume(SDL_Event &event) {
    switch (event.type) {
    case SDL_EVENT_KEY_DOWN: {
        if (event.key.key == SDLK_UP) {
            if (m_LastAdvancedDirection == DIRECTION_RIGHT ||
                m_LastAdvancedDirection == DIRECTION_LEFT)
            {
                m_Direction = DIRECTION_UP;
            }
        }
        else if (event.key.key == SDLK_DOWN) {
            if (m_LastAdvancedDirection == DIRECTION_RIGHT ||
                m_LastAdvancedDirection == DIRECTION_LEFT)
            {
                m_Direction = DIRECTION_DOWN;
            }
        }
        else if (event.key.key == SDLK_RIGHT) {
            if (m_LastAdvancedDirection == DIRECTION_UP ||
                m_LastAdvancedDirection == DIRECTION_DOWN)
            {
                m_Direction = DIRECTION_RIGHT;
            }
        }
        else if (event.key.key == SDLK_LEFT) {
            if (m_LastAdvancedDirection == DIRECTION_UP ||
                m_LastAdvancedDirection == DIRECTION_DOWN)
            {
                m_Direction = DIRECTION_LEFT;
            }
        }
        else if (event.key.key == SDLK_ESCAPE) {
            m_IsPaused = true;
        }
    }

    default:;
    }
}

void Snake::PlaceFood() {
    uint32_t bit0_counts[m_Height];
    uint32_t bit0_count_total = 0;

    // count bits
    for (size_t y = 0; y < m_Height; y++) {
        uint32_t bit1_count = 0;

        uint64_t bitmap_row = m_BodyBitmap[y];
        while (bitmap_row != 0) {
            bitmap_row = bitmap_row & (bitmap_row - 1);
            bit1_count += 1;
        }

        uint32_t bit0_count = m_Width - bit1_count;
        bit0_counts[y] = bit0_count;
        bit0_count_total += bit0_count;
    }


    std::uniform_int_distribution<int32_t> m_Dist(0, bit0_count_total-1);
    size_t bit0_index = m_Dist(m_Rng);
    size_t bit0_x;
    size_t bit0_y;

    // find y
    for (size_t y = 0; y < m_Height; y++) {
        if (bit0_index < bit0_counts[y]) {
            bit0_y = y;
            break;
        }
        bit0_index -= bit0_counts[y];
    }

    // find x
    bit0_x = 0;
    uint64_t bitmap_row_not = ~m_BodyBitmap[bit0_y];
    for (size_t x = 0; x < m_Width; x++) {
        if (bit0_index > 0) {
            if (bitmap_row_not & 1) {
                bit0_index -= 1;
            }
            bitmap_row_not >>= 1;
            bit0_x++;
        }
    }
    // Todo: bugfix this
    if (bit0_index != 0) {
        printf("BUG!!!\n\tbit0_index = %lu, bit0_x = %lu, bit0_y =%lu\n",
                bit0_index, bit0_x, bit0_y);
    }

    m_FoodPosition = {bit0_x, bit0_y};
}

void Snake::Draw(RenderGroup &render_group) {
    /* draw snake */
    // 1) if tail > head: advance to end first
    size_t tail = m_Tail;
    if (tail > m_Head) {
        while (tail < m_Width) {
            V3F32 pos = {m_BodyPositions[tail].x + 0.1f, m_BodyPositions[tail].y + 0.1f, 1};
            V2F32 dim = {0.8f, 0.8f};
            V3F32 color = {0.3f, 0.3f, 0.3f};
            render_group.PushRectangle(pos, dim, color);
            tail++;
        }
        tail = 0;
    }
    // 2) advance to head
    while (tail <= m_Head) {
        V3F32 pos = {m_BodyPositions[tail].x + 0.1f, m_BodyPositions[tail].y + 0.1f, 1};
        V2F32 dim = {0.8f, 0.8f};
        V3F32 color = {0.3f, 0.3f, 0.3f};
        render_group.PushRectangle(pos, dim, color);
        tail++;
    }


    /* draw food */
    V3F32 pos = {m_FoodPosition.x + 0.1f, m_FoodPosition.y + 0.1f, 1};
    V2F32 dim = {0.8f, 0.8f};
    V3F32 color = {0.3f, 0.6f, 0.4f};
    render_group.PushRectangle(pos, dim, color);
}

void Snake::DoImgui() {
    if (m_IsPaused) {
        ImGui::Begin("SnakePause");
        if (ImGui::Button("Resume")) {
            m_IsPaused = false;
        }
        if (ImGui::Button("Exit")) {
            m_IsRunning = false;
        }
        ImGui::End();
    }
}

