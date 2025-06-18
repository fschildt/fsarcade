#pragma once

#include <games/Game.hpp>
#include <unordered_set>

namespace std {
    template <>
    struct hash<V2ST> {
        size_t operator()(const V2ST& v) const {
            size_t h1 = hash<size_t>{}(v.x);
            size_t h2 = hash<size_t>{}(v.y);
            return h1 ^ (h2 << 1);
        }
    };
}

class Minesweeper : public Game {
    public:
        Minesweeper();
        ~Minesweeper() = default;

        bool Update(std::vector<SDL_Event> &events, RenderGroup &render_group) override;

        void ProcessEventDuringPause(SDL_Event &event, RenderGroup &render_group);
        void ProcessEventDuringResume(SDL_Event &event, RenderGroup &render_group);


    private:
        void Uncover(int32_t x, int32_t y);
        void ToggleFlag(int32_t x, int32_t y);

        bool IsCovered(int32_t x, int32_t y);
        bool IsFlagged(int32_t x, int32_t y);
        bool IsMine(int32_t x, int32_t y);

        int32_t CountAdjacentMines(int32_t x, int32_t y);

        V2F32 ScreenPosToViewPos(V2F32 screen_pos, RenderGroup &render_group);


    private:
        void DrawPauseMenu(RenderGroup &render_group);
        void DrawBoard(RenderGroup &render_group);


    private:
        static constexpr int32_t MAX_MAP_HEIGHT = 32;
        static constexpr int32_t MAX_MAP_WIDTH = 32;


    private:
        bool m_IsRunning = true;
        bool m_IsPaused = false;

        float m_WorldWidth = 4.0f;
        float m_WorldHeight = 3.0f;

        int32_t m_MapWidth = 16;
        int32_t m_MapHeight = 16;

        V2F32 m_MapViewPos;
        V2F32 m_CellOuterViewSize;
        V2F32 m_CellInnerViewSize;

        std::unordered_set<V2ST> m_CoveredPositions;
        std::unordered_set<V2ST> m_FlaggedPositions;
        std::vector<V2ST> m_UncoveredPositions;
        uint32_t m_IsCoveredBitmap[MAX_MAP_HEIGHT];
        uint32_t m_IsFlaggedBitmap[MAX_MAP_HEIGHT];
        uint32_t m_IsMineBitmap[MAX_MAP_HEIGHT];
};


