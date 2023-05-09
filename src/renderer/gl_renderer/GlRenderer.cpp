#include <renderer/gl_renderer/GlRenderer.h>
#include <math/math.h>
#include <renderer/Renderer.h>
#include <renderer/gl_renderer/GlVertexBuffer.h>

#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <GL/gl.h>
#include <SDL2/SDL_video.h>
#include <assert.h>

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
    SDL_GL_DeleteContext(m_Context);
}

bool GlRenderer::Init(SDL_Window *sdl_window, uint8_t *memory, size_t memory_size) {
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    void *sdl_gl_context = SDL_GL_CreateContext(sdl_window);
    if (!sdl_gl_context) {
        return false;
    }

    GLenum err = glewInit();
    if (GLEW_OK != err)
    {
        fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
        SDL_GL_DeleteContext(sdl_gl_context);
        return false;
    }

    printf("%s\n", glGetString(GL_VERSION));

    // During init, enable debug output
#if 0
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(MessageCallback, 0);
#endif

    GLuint vertex_array;
    glGenVertexArrays(1, &vertex_array);
    glBindVertexArray(vertex_array);

    if (!m_Shader.InitProgram()) {
        return false;
    }

    Stack stack(memory, memory_size);
    m_VertexBuffer.Init(&stack);
    m_IndexBuffer.Init(&stack);

    m_Window = sdl_window;
    m_Context = sdl_gl_context;
    return true;
}

void GlRenderer::Draw(RenderGroup *render_group, int width, int height) {
    glViewport(0, 0, width, height);
    render_group->Sort();

    float last_z = -2;

    int32_t entry_count = render_group->mRenderEntryCount;
    RenderSortEntry *sort_entry = (RenderSortEntry*)render_group->mRenderSortEntries;
    RenderSortEntry *max_sort_entry_plus_1 = sort_entry + entry_count;
    while (sort_entry < max_sort_entry_plus_1) {
        RenderEntryType *type = (RenderEntryType*)sort_entry->value;
        switch (*type) {
            case RenderEntryType_Clear: {
                RenderEntry_Clear *clear = (RenderEntry_Clear*)type;
                glClearColor(clear->color.g, clear->color.g, clear->color.b, 1.f);
                glClear(GL_COLOR_BUFFER_BIT);
                last_z = -1;
            } break;

            case RenderEntryType_Rectangle: {
                RenderEntry_Rectangle *rect = (RenderEntry_Rectangle*)type;

                if (rect->pos.z > last_z && last_z >= 0.f) {
                    DrawBatch();
                    last_z = rect->pos.z;
                }

                // convert pos from [0, rg_max] to [-1, 1]
                // convert dim from [0, rg_max] to [ 0, 2]
                float rg_xmax = render_group->m_XMax;
                float rg_ymax = render_group->m_YMax;
                V3 pos = rect->pos;
                V2 dim = rect->dim;
                pos.x = 2*(rect->pos.x / rg_xmax) - 1;
                pos.y = 2*(rect->pos.y / rg_ymax) - 1;
                dim.x = 2*(rect->dim.x / rg_xmax);
                dim.y = 2*(rect->dim.y / rg_ymax);

                m_VertexBuffer.PushRectangle(pos, dim, rect->color);
                m_IndexBuffer.PushRectangle();
            } break;

            default: assert(0);
        }

        sort_entry++;
    }

    DrawBatch();
}

void GlRenderer::Present() {
    SDL_GL_SwapWindow(m_Window);
}

void GlRenderer::DrawBatch() {
    if (m_IndexBuffer.GetCount() == 0) {
        return;
    }

    // draw
    m_VertexBuffer.TransferData();
    m_IndexBuffer.TransferData();
    m_Shader.UseProgram();
    int index_count = m_IndexBuffer.GetCount();
    glDrawElements(GL_TRIANGLES, index_count, GL_UNSIGNED_INT, 0);

    // reset
    m_VertexBuffer.Reset();
    m_IndexBuffer.Reset();
    glUseProgram(0);
}


