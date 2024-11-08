#pragma once

#include <renderer/Renderer.hpp>
#include <renderer/gl_renderer/GlVertexBuffer.hpp>
#include <renderer/gl_renderer/GlIndexBuffer.hpp>
#include <renderer/gl_renderer/GlShader.hpp>

class GlRenderer : public Renderer {
public:
    bool Init(SDL_Window *window) override;
    ~GlRenderer() override;

    void Draw(RenderGroup& render_group, int width, int height) override;
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
