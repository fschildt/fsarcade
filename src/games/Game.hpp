#pragma once

#include <basic/defs.hpp>

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
    static Game *Select(GameType type);

public:
    virtual void Init() = 0;
    virtual void Update(GameInput& input, RenderGroup& render_group) = 0;
};

