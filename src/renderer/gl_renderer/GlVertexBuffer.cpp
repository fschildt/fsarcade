#include <renderer/gl_renderer/GlVertexBuffer.hpp>

#include <GL/glew.h>
#include <string.h>

#include <stdio.h>

void GlVertexBuffer::Init() {
    m_Vertices.reserve(16384);
    glGenBuffers(1, &m_Id);
    glBindBuffer(GL_ARRAY_BUFFER, m_Id);
}

void GlVertexBuffer::Reset() {
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    m_Vertices.clear();
}

float *GlVertexBuffer::GetData() {
    return (float*)m_Vertices.data();
}

uint32_t GlVertexBuffer::GetCount() {
    return m_Vertices.size();
}

void GlVertexBuffer::PushRectangle(V3 pos, V2 dim, V3 color) {
    V3 positions[4] = {
        V3(pos.x,         pos.y,         pos.z),
        V3(pos.x + dim.x, pos.y,         pos.z),
        V3(pos.x + dim.x, pos.y + dim.y, pos.z),
        V3(pos.x,         pos.y + dim.y, pos.z),
    };

    for (int i = 0; i < 4; i++) {
        GlVertex vertex = {};
        vertex.pos = positions[i];
        vertex.color = color;
        m_Vertices.push_back(vertex);
    }
}

void GlVertexBuffer::TransferData() {
    size_t size = m_Vertices.size() * sizeof(GlVertex);
    GLsizei stride = sizeof(GlVertex);
    const void *offset_color = (const void*)(3*sizeof(float));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, 0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, offset_color);
    glBufferData(GL_ARRAY_BUFFER, size, m_Vertices.data(), GL_STATIC_DRAW);
}

