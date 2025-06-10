#pragma once

#include <renderer/Renderer.hpp>
#include <renderer/gl_renderer/GlVertexBuffer.hpp>
#include <renderer/gl_renderer/GlIndexBuffer.hpp>
#include <renderer/gl_renderer/GlShader.hpp>

class GlRenderer : public Renderer {
public:
    GlRenderer(SDL_Window *window);
    ~GlRenderer() override;

    bool Init() override;
    void Draw(RenderGroup& render_group) override;
    void Present() override;

private:
    void DrawBatch();

private:
    SDL_Window *m_Window;
    SDL_GLContext m_Context;

    GlVertexBuffer m_RectangleVertexBuffer;
    GlIndexBuffer m_RectangleIndexBuffer;
    GlShader m_RectangleShader;

    GlVertexBuffer m_TextVertexBuffer;
    GlIndexBuffer m_TextIndexBuffer;
    GlShader m_TextShader;
};
