#pragma once

#include <renderer/RenderGroup.hpp>
#include <games/Game.hpp>

#include <random>


class Snake : public Game {
public:
    enum Direction : int32_t {
        DIRECTION_UP,
        DIRECTION_DOWN,
        DIRECTION_LEFT,
        DIRECTION_RIGHT,
    };


public:
    Snake();
    bool Update(std::vector<SDL_Event> &events, RenderGroup &render_group) override;


private:
    void ProcessEventDuringPause(SDL_Event &event);
    void ProcessEventDuringResume(SDL_Event &event);

    void MaybeMoveSnake(float dt_in_seconds);
    void SpawnFood();

    void Draw(RenderGroup &render_group);
    void DoImgui();



private:
    static constexpr int32_t MAX_MAP_WIDTH = 16;
    static constexpr int32_t MAX_MAP_HEIGHT = 16;

    bool m_IsPaused;
    bool m_IsRunning;

    float m_DtInSecondsRemaining;
    uint64_t m_LastMillisecondsSinceT0;

    float m_TilesPerSecond;
    Direction m_Direction;
    Direction m_LastAdvancedDirection;

    int32_t m_MapWidth;
    int32_t m_MapHeight;
    int32_t m_Tail;
    int32_t m_Head;
    uint64_t m_BodyBitmap[MAX_MAP_HEIGHT];
    V2I32 m_BodyPositions[MAX_MAP_WIDTH * MAX_MAP_HEIGHT];
    V2I32 m_FoodPosition;

    std::mt19937 m_Rng;
    std::uniform_int_distribution<int32_t> m_Dist;
};


