#include <basic/defs.hpp>
#include <renderer/RenderGroup.hpp>
#include <renderer/gl_renderer/GlRenderer.hpp>
#include <basic/math.hpp>
#include <renderer/Renderer.hpp>
#include <renderer/gl_renderer/GlVertexBuffer.hpp>

#include <SDL3/SDL.h>
#include <SDL3/SDL_video.h>
#include <GL/glew.h>
#include <GL/gl.h>
#include <assert.h>
#include <cstdio>

void GLAPIENTRY
MessageCallback(GLenum source,
                GLenum type,
                GLuint id,
                GLenum severity,
                GLsizei length,
                const GLchar* message,
                const void* userParam)
{
    fprintf( stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
             ( type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "" ),
             type, severity, message );
}

GlRenderer::~GlRenderer() {
    SDL_GL_DestroyContext(m_Context);
}

bool GlRenderer::Init(SDL_Window *sdl_window) {
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GLContext sdl_gl_context = SDL_GL_CreateContext(sdl_window);
    if (!sdl_gl_context) {
        return false;
    }

    GLenum err = glewInit();
    if (GLEW_OK != err)
    {
        fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
        SDL_GL_DestroyContext(sdl_gl_context);
        return false;
    }

#if 0
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(MessageCallback, 0);
#endif

    GLuint vertex_array;
    glGenVertexArrays(1, &vertex_array);
    glBindVertexArray(vertex_array);


    // init rectangle rendering
    if (!m_RectangleShader.InitProgram()) {
        return false;
    }
    m_RectangleVertexBuffer.Init();
    m_RectangleIndexBuffer.Init();

    // init text rendering
    if (!m_TextShader.InitProgram()) {
        return false;
    }
    m_TextVertexBuffer.Init();
    m_TextIndexBuffer.Init();


    m_Window = sdl_window;
    m_Context = sdl_gl_context;
    return true;
}

void GlRenderer::Draw(RenderGroup& render_group, int width, int height) {
    glViewport(0, 0, width, height);
    render_group.Sort();

    // find values which allows us to scale and center properly
    float screen_ratio = (float) width / (float) height;
    float rg_xmax = render_group.m_XMax;
    float rg_ymax = render_group.m_YMax;

    // draw batches
    float last_z = -2;
    for (auto [z, entity_index] : render_group.m_RSortEntries) {
        REntity& render_entity = render_group.m_REntities[entity_index];
        switch (render_entity.type) {
            case REntityType_Clear: {
                REntity_Clear& clear = render_entity.clear;
                glClearColor(clear.color.r, clear.color.g, clear.color.b, 1.f);
                glClear(GL_COLOR_BUFFER_BIT);
                last_z = -1;
            } break;

            case REntityType_Rectangle: {
                REntity_Rectangle& rect = render_entity.rect;
                if (rect.pos.z > last_z && last_z >= 0.f) {
                    DrawBatch();
                    last_z = rect.pos.z;
                }

                // convert pos from [0, rg_max] to [-1, 1]
                // convert dim from [0, rg_max] to [ 0, 2]
                V3 pos = rect.pos;
                V2 dim = rect.dim;
                pos.x = 2*(rect.pos.x / rg_xmax) - 1;
                pos.y = 2*(rect.pos.y / rg_ymax) - 1;
                dim.x = 2*(rect.dim.x / rg_xmax);
                dim.y = 2*(rect.dim.y / rg_ymax);

                m_RectangleVertexBuffer.PushRectangle(pos, dim, rect.color);
                m_RectangleIndexBuffer.PushRectangle();
            }

            case REntityType_Bitmap: {
            } break;

            InvalidDefaultCase;
        }
    }


    DrawBatch();
}

void GlRenderer::Present() {
    SDL_GL_SwapWindow(m_Window);
}

void GlRenderer::DrawBatch() {
    int rectangle_index_count = m_RectangleIndexBuffer.GetCount();
    int text_index_count = m_TextIndexBuffer.GetCount();

    if (rectangle_index_count) {
        // setup
        m_RectangleVertexBuffer.TransferData();
        m_RectangleIndexBuffer.TransferData();
        m_RectangleShader.UseProgram();

        // draw
        glDrawElements(GL_TRIANGLES, rectangle_index_count, GL_UNSIGNED_INT, 0);

        // reset
        m_RectangleVertexBuffer.Reset();
        m_RectangleIndexBuffer.Reset();
    }
    
    if (text_index_count) {
        // setup
        m_TextVertexBuffer.TransferData();
        m_TextIndexBuffer.TransferData();
        m_TextShader.UseProgram();

        // draw
        glDrawElements(GL_TRIANGLES, text_index_count, GL_UNSIGNED_INT, 0);

        // reset
        m_TextVertexBuffer.Reset();
        m_TextIndexBuffer.Reset();
    }
}


