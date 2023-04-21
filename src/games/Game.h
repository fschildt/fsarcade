#pragma once

#include <stdint.h>

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

    GameController controller1;
    GameController controller2;

    void Gather(SDL_Window *window);
};

class Game {
public:
    enum GameType {
        NO_GAME,
        TETRIS,
    };
    static Game *Select(GameType);

public:
    virtual void Init() = 0;
    virtual void Update(const GameInput *input, RenderGroup *render_group) = 0;
};

