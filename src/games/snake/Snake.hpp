#pragma once

#include <renderer/RenderGroup.hpp>
#include <games/Game.hpp>

#include <random>


struct V2ST {
    V2ST() = default;
    V2ST(size_t x, size_t y);
    bool operator==(V2ST &b) {return x == b.x && y == b.y;}
    size_t x;
    size_t y;
};

class Snake : public Game {
public:
    enum Direction {
        DIRECTION_UP,
        DIRECTION_DOWN,
        DIRECTION_LEFT,
        DIRECTION_RIGHT,
    };


public:
    Snake();
    void Init() override;
    bool Update(std::vector<SDL_Event> &events, RenderGroup &render_group) override;


private:
    void ProcessEventDuringPause(SDL_Event &event);
    void ProcessEventDuringResume(SDL_Event &event);

    void MaybeMoveSnake(float dt_in_seconds);
    void MoveSnake();

    void PlaceFood();

    void Draw(RenderGroup &render_group);
    void DoImgui();


private:
    static constexpr size_t MAX_WIDTH = 64; // can't exceed 64, see m_Bitmap
    static constexpr size_t MAX_HEIGHT = 64;

    float m_DtInSecondsRemaining;
    float m_LastMillisecondsSinceT0;
    float m_TilesPerSecond;

    bool m_IsPaused;
    bool m_IsRunning;

    Direction m_Direction;
    Direction m_LastAdvancedDirection;

    size_t m_Width;
    size_t m_Height;
    size_t m_Head;
    size_t m_Tail;
    uint64_t m_BodyBitmap[MAX_HEIGHT];
    V2ST m_BodyPositions[MAX_WIDTH * MAX_HEIGHT];

    V2ST m_FoodPosition;

    std::mt19937 m_Rng;
};


