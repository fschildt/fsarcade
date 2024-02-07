#include <renderer/gl_renderer/GlRenderer.hpp>
#include <math/math.hpp>
#include <renderer/Renderer.hpp>
#include <renderer/gl_renderer/GlVertexBuffer.hpp>

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

#if 0
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(MessageCallback, 0);
#endif

    GLuint vertex_array;
    glGenVertexArrays(1, &vertex_array);
    glBindVertexArray(vertex_array);


    MemArena arena(memory, memory_size);

    // init rectangle rendering
    if (!m_RectangleShader.InitProgram()) {
        return false;
    }
    m_RectangleVertexBuffer.Init(&arena);
    m_RectangleIndexBuffer.Init(&arena);

    // init text rendering
    if (!m_TextShader.InitProgram()) {
        return false;
    }
    m_TextVertexBuffer.Init(&arena);
    m_TextIndexBuffer.Init(&arena);


    m_Window = sdl_window;
    m_Context = sdl_gl_context;
    return true;
}

void GlRenderer::Draw(RenderGroup *render_group, int width, int height) {
    glViewport(0, 0, width, height);
    render_group->Sort();



    // find values which allows us to scale and center properly

    float screen_ratio = (float) width / (float) height;
    float rg_xmax = render_group->m_XMax;
    float rg_ymax = render_group->m_YMax;
    float potential_width_usage  = width / rg_xmax;
    float potential_height_usage = height / rg_ymax;

    float xoff;
    float yoff;
    float usable_width;
    float usable_height;
    if (potential_width_usage >= potential_height_usage) {
        usable_width = width;
        usable_height = width * screen_ratio;
        xoff = 0;
        yoff = (height - usable_height) / 2;
    } else {
        usable_height = height;
        usable_width = height / screen_ratio;
        xoff = (width - usable_width) / 2;
        yoff = 0;
    }



    // draw batches

    float last_z = -2;

    int32_t entry_count = render_group->mRenderEntryCount;
    if (entry_count == 0) return;
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

                // convert pos from [0, rg_max] to [{xoff,yoff}, {xoff+usable_width, yoff+usable_height}]

                // convert pos from [0, rg_max] to [-1, 1]
                // convert dim from [0, rg_max] to [ 0, 2]
                V3 pos = rect->pos;
                V2 dim = rect->dim;
                pos.x = 2*(rect->pos.x / rg_xmax) - 1;
                pos.y = 2*(rect->pos.y / rg_ymax) - 1;
                dim.x = 2*(rect->dim.x / rg_xmax);
                dim.y = 2*(rect->dim.y / rg_ymax);

                m_RectangleVertexBuffer.PushRectangle(pos, dim, rect->color);
                m_RectangleIndexBuffer.PushRectangle();
            } break;

            case RenderEntryType_Bitmap: {
                RenderEntry_Bitmap *bitmap = (RenderEntry_Bitmap*)type;

                if (bitmap->pos.z > last_z && last_z >= 0.f) {
                    DrawBatch();
                    last_z = bitmap->pos.z;
                }

#if 0
                V3 pos = {};
                V2 dim = {};
                char c = 'A';
                m_TextVertexBuffer.PushVertices();
                m_TextIndexBuffer.PushIndices();
#endif
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


