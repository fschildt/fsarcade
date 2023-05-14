#pragma once

#include <fsarcade_defs.h>

class Stack;
class RenderGroup;
struct SDL_Window;

struct GameController {
    bool ActionA = false;
    bool ActionB = false;
    bool MoveUp = false;
    bool MoveDown = false;
    bool MoveRight = false;
    bool MoveLeft = false;
    bool Confirm = false;
    bool Exit = false;
};

struct GameGlyph {
    int x;
};

struct GameFont {
    GameGlyph glyphs[2];
};

struct GameInput {
    bool quit;
    uint64_t dt;
    GameController controller;
    void Gather(SDL_Window *window);
};


class Game {
public:
    enum GameType {
        NO_GAME,
        TETRIS,
    };
    static Game *Select(GameType type, uint8_t *memory, size_t memory_size);

public:
    virtual void Init(uint8_t *memory, size_t memory_size, GameFont *font) = 0;
    virtual void Update(const GameInput *input, RenderGroup *render_group) = 0;
};

