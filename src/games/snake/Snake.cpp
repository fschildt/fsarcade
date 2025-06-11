#include <games/snake/Snake.hpp>
#include <imgui.h>


Snake::Snake () {
    m_IsPaused = false;
    m_IsRunning = true;

    m_DtInSecondsRemaining = 0.0f;
    m_LastMillisecondsSinceT0 = SDL_GetTicks();

    m_TilesPerSecond = 4.0f;
    m_Direction = DIRECTION_RIGHT;
    m_LastAdvancedDirection = DIRECTION_RIGHT;

    m_MapWidth = 16;
    m_MapHeight = 16;
    assert(MAX_MAP_WIDTH <= 64); // m_BodyBitmap is uint64_t[]. We can't exceed that!
    assert(MAX_MAP_HEIGHT <= 64);
    assert(m_MapWidth <= MAX_MAP_WIDTH);
    assert(m_MapHeight <= MAX_MAP_WIDTH);

    m_Tail = 0;
    m_Head = 1;
    memset(m_BodyBitmap, 0, sizeof(m_BodyBitmap));

    size_t head_x = m_MapWidth / 2;
    size_t head_y = m_MapHeight / 2;
    m_BodyPositions[0] = {head_x -1, head_y};
    m_BodyPositions[1] = {head_x, head_y};

    m_Rng = std::mt19937((std::random_device()()));
    m_Dist = std::uniform_int_distribution<int32_t>(0, m_MapWidth * m_MapHeight - 3);

    SpawnFood();
}

bool Snake::Update(std::vector<SDL_Event> &events, RenderGroup &render_group) {
    uint64_t milliseconds_since_t0 = SDL_GetTicks();
    float dt_in_seconds = (milliseconds_since_t0 - m_LastMillisecondsSinceT0) / 1000.0f;
    m_LastMillisecondsSinceT0 = milliseconds_since_t0;


    V3F32 clear_color = V3F32(0.3f, 0.3f, 0.3f);
    render_group.SetSize(4.0f, 3.0f);
    render_group.PushClear(clear_color);


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

    if (!m_IsPaused) {
        MaybeMoveSnake(dt_in_seconds);
    }


    Draw(render_group);
    DoImgui();

    return m_IsRunning;
}

void Snake::MaybeMoveSnake(float dt_in_seconds) {
    float dt_in_seconds_to_use = m_DtInSecondsRemaining + dt_in_seconds;
    float tiles_per_second = m_TilesPerSecond;
    float seconds_per_tile = 1.0f / tiles_per_second;
    while (dt_in_seconds_to_use > seconds_per_tile) {
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
        if (head_pos.y >= m_MapHeight || head_pos.x >= m_MapWidth) {
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


        // advance head
        size_t max_positions = sizeof(m_BodyPositions) / sizeof(m_BodyPositions[0]);
        m_Head += 1;
        if (m_Head >= max_positions) {
            m_Head = 0;
        }

        m_BodyPositions[m_Head] = head_pos;
        m_BodyBitmap[head_pos.y] |= (1 << head_pos.x);


        if (m_BodyPositions[m_Head] == m_FoodPosition) {
            SpawnFood();
        }
        else {
            // advance tail
            V2ST tail_pos = m_BodyPositions[m_Tail];
            m_BodyBitmap[tail_pos.y] &= ~(1 << tail_pos.x);

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

void Snake::SpawnFood() {
    uint32_t bit0_counts[m_MapHeight];
    uint32_t bit0_count_total = 0;

    // count bits
    for (size_t y = 0; y < m_MapHeight; y++) {
        uint32_t bit1_count = 0;

        uint64_t bitmap_row = m_BodyBitmap[y];
        while (bitmap_row != 0) {
            bitmap_row = bitmap_row & (bitmap_row - 1);
            bit1_count += 1;
        }

        uint32_t bit0_count = m_MapWidth - bit1_count;
        bit0_counts[y] = bit0_count;
        bit0_count_total += bit0_count;
    }

    if (bit0_count_total == 0) {
        return;
    }

    m_Dist.param(std::uniform_int_distribution<int32_t>::param_type(0, bit0_count_total - 1));
    size_t bit0_index = m_Dist(m_Rng);
    size_t bit0_x = 0;
    size_t bit0_y = 0;

    // find y
    for (size_t y = 0; y < m_MapHeight; y++) {
        if (bit0_index < bit0_counts[y]) {
            bit0_y = y;
            break;
        }
        bit0_index -= bit0_counts[y];
    }

    // find x
    uint64_t bitmap_row_not = ~m_BodyBitmap[bit0_y];
    for (size_t x = 0; x < m_MapWidth; x++) {
        if (bitmap_row_not & 1) {
            if (bit0_index == 0) {
                bit0_x = x;
                break;
            }
            bit0_index--;
        }
        bitmap_row_not >>= 1;
    }

    m_FoodPosition = {bit0_x, bit0_y};
}

void Snake::Draw(RenderGroup &render_group) {
    float world_width = 4.0f;
    float world_height = 3.0f;

    float tile_size = (world_width / 2) / MAX_MAP_WIDTH;
    float bodypart_size = 0.8f * tile_size;
    float bodypart_offset = (tile_size - bodypart_size) / 2;

    float map_width = tile_size * m_MapWidth;
    float map_height = tile_size * m_MapHeight;
    float map_x = (world_width - map_width) / 2;
    float map_y = (world_height - map_height) / 2;

    size_t max_positions = sizeof(m_BodyPositions) / sizeof(m_BodyPositions[0]);


    /* draw map background */
    V3F32 map_world_pos = {map_x, map_y, 1.0f};
    V2F32 map_world_dim = {map_width, map_height};
    V3F32 bg_color = {0.0f, 0.0f, 0.0f};
    render_group.PushRectangle(map_world_pos, map_world_dim, bg_color);


    /* draw snake */
    // 1) if tail > head: advance to end first
    size_t tail = m_Tail;
    if (tail > m_Head) {
        while (tail < max_positions) {
            V3F32 local_pos = {
                m_BodyPositions[tail].x * tile_size + bodypart_offset,
                m_BodyPositions[tail].y * tile_size + bodypart_offset,
                2.0f
            };
            V2F32 local_dim = {bodypart_size, bodypart_size};

            V3F32 world_pos = {
                map_world_pos.x + local_pos.x,
                map_world_pos.y + local_pos.y,
                2.0f
            };
            V2F32 world_dim = local_dim;

            V3F32 color = {0.3f, 0.3f, 0.3f};
            render_group.PushRectangle(world_pos, world_dim, color);
            tail++;
        }
        tail = 0;
    }
    // 2) advance to head
    while (tail <= m_Head) {
        V3F32 local_pos = {
            m_BodyPositions[tail].x * tile_size + bodypart_offset,
            m_BodyPositions[tail].y * tile_size + bodypart_offset,
            2.0f
        };
        V2F32 local_dim = {bodypart_size, bodypart_size};

        V3F32 world_pos = {
            map_world_pos.x + local_pos.x,
            map_world_pos.y + local_pos.y,
            2.0f
        };
        V2F32 world_dim = local_dim;

        V3F32 color = {0.3f, 0.3f, 0.3f};
        render_group.PushRectangle(world_pos, world_dim, color);
        tail++;
    }


    /* draw food */
    V3F32 pos = {
        map_world_pos.x + m_FoodPosition.x * tile_size + bodypart_offset,
        map_world_pos.y + m_FoodPosition.y * tile_size + bodypart_offset,
        2.0f
    };
    V2F32 dim = {bodypart_size, bodypart_size};
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

