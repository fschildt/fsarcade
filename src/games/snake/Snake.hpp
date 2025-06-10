#pragma once

#include <renderer/RenderGroup.hpp>
#include <games/Game.hpp>

#include <random>


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
    void Init();


private:
    static constexpr size_t MAX_MAP_WIDTH = 24; // can't exceed 64, see m_Bitmap
    static constexpr size_t MAX_MAP_HEIGHT = 24;

    bool m_IsInitialized = false;
    bool m_IsPaused = false;
    bool m_IsRunning = true;

    float m_DtInSecondsRemaining = 0.0f;
    float m_LastMillisecondsSinceT0 = SDL_GetTicks();

    float m_TilesPerSecond = 4.0f;
    Direction m_Direction = DIRECTION_RIGHT;
    Direction m_LastAdvancedDirection = DIRECTION_RIGHT;

    size_t m_Width = 16;
    size_t m_Height = 16;
    size_t m_Tail = 0;
    size_t m_Head = 1;
    uint64_t m_BodyBitmap[MAX_MAP_HEIGHT]{};
    V2ST m_BodyPositions[MAX_MAP_WIDTH * MAX_MAP_HEIGHT];
    V2ST m_FoodPosition = {};

    std::mt19937 m_Rng;
};


