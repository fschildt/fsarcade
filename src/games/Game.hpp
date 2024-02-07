#pragma once

#include <defs.hpp>

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

struct GameInput {
    bool quit;
    float dt;
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
    virtual void Init(uint8_t *memory, size_t memory_size) = 0;
    virtual void Update(GameInput *input, RenderGroup *render_group) = 0;
};

