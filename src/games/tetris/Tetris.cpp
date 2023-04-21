#include <games/tetris/Tetris.h>

#include <renderer/Renderer.h>

#include <stdio.h>
#include <string.h>

void Tetris::Init() {
    m_PlayerStates[0].Init();
}

void Tetris::Update(const GameInput *input, RenderGroup *render_group) {
    V3 clear_color = V3(0.f, 0.f, 0.f);
    render_group->SetScale(10, 20);
    render_group->PushClear(clear_color);

    m_PlayerStates[0].Update(&input->controller1, render_group);
}

