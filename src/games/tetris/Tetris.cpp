#include <games/tetris/Tetris.h>

#include <renderer/Renderer.h>

#include <stdio.h>
#include <string.h>

Tetris::Tetris(void *memory, size_t memory_size) {
    // we're not using extra memory atm
}

void Tetris::Init() {
    m_PlayerState.Init();
}

void Tetris::Update(const GameInput *input, RenderGroup *render_group) {
    V3 clear_color = V3(0.f, 0.f, 0.f);
    render_group->SetSize(10, 20);
    render_group->PushClear(clear_color);

    m_PlayerState.Update(&input->controller, input->dt, render_group);
}

