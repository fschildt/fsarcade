#pragma once

#include <renderer/Renderer.h>
#include <renderer/gl_renderer/GlVertexBuffer.h>
#include <renderer/gl_renderer/GlIndexBuffer.h>
#include <renderer/gl_renderer/GlShader.h>

class GlRenderer : public Renderer {
public:
    bool Init(SDL_Window *window, uint8_t *memory, size_t memory_size) override;
    ~GlRenderer() override;

    void Draw(RenderGroup *render_group, int width, int height) override;
    void Present() override;

private:
    void DrawBatch();

private:
    SDL_Window *m_Window;
    void *m_Context = 0;

    GlVertexBuffer m_RectangleVertexBuffer;
    GlIndexBuffer m_RectangleIndexBuffer;
    GlShader m_RectangleShader;

    GlVertexBuffer m_TextVertexBuffer;
    GlIndexBuffer m_TextIndexBuffer;
    GlShader m_TextShader;
};
