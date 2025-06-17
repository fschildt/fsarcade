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

GlRenderer::GlRenderer(SDL_Window *window)
        : m_Window(window), m_Context(nullptr) {
}

bool GlRenderer::Init() {
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GLContext sdl_gl_context = SDL_GL_CreateContext(m_Window);
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


    m_Context = sdl_gl_context;
    return true;
}

void GlRenderer::Draw(RenderGroup& render_group) {
    render_group.Sort();

    float screen_width = render_group.m_ScreenWidth;
    float screen_height = render_group.m_ScreenHeight;
    float camera_width = render_group.m_CameraWidth;
    float camera_height = render_group.m_CameraHeight;


    glViewport(0, 0, render_group.m_ScreenWidth, render_group.m_ScreenHeight);
    glClearColor(render_group.m_ClearColor.x,
                 render_group.m_ClearColor.y,
                 render_group.m_ClearColor.z,
                 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);


    // viewport space
    float scale = render_group.GetScale();
    float viewport_width = camera_width * scale;
    float viewport_height = camera_height * scale;
    float viewport_x0 = (screen_width - viewport_width) / 2;
    float viewport_y0 = (screen_height - viewport_height) / 2;
    glViewport(viewport_x0, viewport_y0, viewport_width, viewport_height);


    // draw batches
    float last_z = -1;
    for (auto [z, entity_index] : render_group.m_RSortEntries) {
        REntity& render_entity = render_group.m_REntities[entity_index];
        switch (render_entity.type) {
            case REntityType_Rectangle: {
                // clip space (from camera space to [-1, 1] for pos and [0, 2] for dim)
                V3F32 pos = render_entity.rect.pos;
                V2F32 dim = render_entity.rect.dim;
                pos.x = 2*(pos.x / camera_width) - 1;
                pos.y = 2*(pos.y / camera_height) - 1;
                dim.x = 2*(dim.x / camera_width);
                dim.y = 2*(dim.y / camera_height);

                if (render_entity.rect.pos.z > last_z) {
                    DrawBatch();
                    last_z = z;
                }

                m_RectangleVertexBuffer.PushRectangle(pos, dim, render_entity.rect.color);
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
        m_RectangleVertexBuffer.TransferData();
        m_RectangleIndexBuffer.TransferData();
        m_RectangleShader.UseProgram();

        glDrawElements(GL_TRIANGLES, rectangle_index_count, GL_UNSIGNED_INT, 0);

        m_RectangleVertexBuffer.Reset();
        m_RectangleIndexBuffer.Reset();
    }
    
    if (text_index_count) {
        m_TextVertexBuffer.TransferData();
        m_TextIndexBuffer.TransferData();
        m_TextShader.UseProgram();

        glDrawElements(GL_TRIANGLES, text_index_count, GL_UNSIGNED_INT, 0);

        m_TextVertexBuffer.Reset();
        m_TextIndexBuffer.Reset();
    }
}


