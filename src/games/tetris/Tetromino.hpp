#pragma once

#include <basic/defs.hpp>
#include <basic/math.hpp>
#include <renderer/RenderGroup.hpp>
#include <games/tetris/Board.hpp>

enum class TetrominoId : uint8_t {
    TETROMINO_O = 0,
    TETROMINO_S,
    TETROMINO_Z,
    TETROMINO_T,
    TETROMINO_L,
    TETROMINO_J,
    TETROMINO_I,
    TETROMINO_ID_COUNT,
    TETROMINO_ID_NONE,
};
enum class TetrominoRotation {Clockwise = 1, CounterClockwise = 3};
enum class TetrominoDirection {Left = -1, Right = 1};


class Tetromino {
public:
    Tetromino() = delete;
    Tetromino(Board &board);
    Tetromino(uint16_t *board_bitmap);

    TetrominoId GetId();
    BoardPos GetPos();
    int32_t GetOri();
    void GetBitmap(uint16_t *bitmap);
    bool IsCollisionWithBoard(); // for last tetromino to check game over

    bool MaybeMoveDown();
    void MaybeMoveHorizontally(TetrominoDirection direction);
    void MaybeRotate(TetrominoRotation rotation);

    void Draw(RenderGroup &render_group) const;


public:
    static bool IsCollisionWithBoard(TetrominoId id, BoardPos pos, int32_t ori, uint16_t *board_bitmap);
    static void GetBitmap(TetrominoId id, BoardPos pos, int32_t ori, uint16_t *bitmap);
    static V3F32 GetColor(TetrominoId id);
    static void Draw(TetrominoId id, int32_t ori, V2F32 pos, float scale, RenderGroup &render_group);


private:
    static TetrominoId GetRandomId();


private:
    TetrominoId m_Id;
    BoardPos m_Pos;
    int32_t m_Ori;
    uint16_t *m_BoardBitmap;
};

