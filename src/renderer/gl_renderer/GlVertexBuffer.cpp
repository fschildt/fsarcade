#include <renderer/gl_renderer/GlVertexBuffer.hpp>

#include <GL/glew.h>
#include <string.h>

#include <stdio.h>
#include <assert.h>

void GlVertexBuffer::Init(MemArena *arena) {
    int32_t max_vertex_count = 16384;
    int32_t vertices_size = max_vertex_count * sizeof(GlVertex);
    GlVertex *vertices = (GlVertex*)arena->Push(vertices_size);

    glGenBuffers(1, &m_Id);
    glBindBuffer(GL_ARRAY_BUFFER, m_Id);
    m_Count = 0;
    m_MaxCount = max_vertex_count;
    m_Vertices = vertices;
}

void GlVertexBuffer::Reset() {
    m_Count = 0;
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
}

float *GlVertexBuffer::GetData() {
    return (float*)m_Vertices;
}

uint32_t GlVertexBuffer::GetCount() {
    return m_Count;
}

void GlVertexBuffer::PushRectangle(V3 pos, V2 dim, V3 color) {
    V3 positions[4] = {
        V3(pos.x,         pos.y,         pos.z),
        V3(pos.x + dim.x, pos.y,         pos.z),
        V3(pos.x + dim.x, pos.y + dim.y, pos.z),
        V3(pos.x,         pos.y + dim.y, pos.z),
    };

    int32_t offset = m_Count;
    for (int i = 0; i < 4; i++) {
        m_Vertices[offset + i].pos = positions[i];
        m_Vertices[offset + i].color = color;
    }

    m_Count += 4;
    assert(m_Count <= m_MaxCount);
}

void GlVertexBuffer::TransferData() {
    size_t size = m_Count * sizeof(GlVertex);
    glBufferData(GL_ARRAY_BUFFER, size, m_Vertices, GL_DYNAMIC_DRAW);

    GLsizei stride = sizeof(GlVertex);
    const void *offset_color = (const void*)(3*sizeof(float));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, 0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, offset_color);
    glBufferData(GL_ARRAY_BUFFER, size, m_Vertices, GL_STATIC_DRAW);
}

