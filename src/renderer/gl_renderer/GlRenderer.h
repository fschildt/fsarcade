#pragma once

#include <renderer/Renderer.h>
#include <renderer/gl_renderer/GlVertexBuffer.h>
#include <renderer/gl_renderer/GlIndexBuffer.h>
#include <renderer/gl_renderer/GlShader.h>

class GlRenderer : public Renderer {
public:
    bool Init(SDL_Window *window, Stack *stack) override;
    ~GlRenderer() override;

    void Draw(RenderGroup *render_group, int width, int height) override;
    void Present() override;

private:
    void DrawBatch();
    V3 Normalize(V3 v);
    V2 Normalize(V2 v);

private:
    SDL_Window *m_Window;
    void *m_Context = 0;

    GlShader m_Shader;
    uint32_t m_VertexArray;
    GlVertexBuffer m_VertexBuffer;
    GlIndexBuffer m_IndexBuffer;
};
