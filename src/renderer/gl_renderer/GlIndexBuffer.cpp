#include <renderer/gl_renderer/GlIndexBuffer.h>

#include <GL/glew.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

void GlIndexBuffer::Init(Stack *stack) {
    int32_t max_index_count = 16384;
    int32_t indices_size = max_index_count * sizeof(GLuint);
    GLuint *indices = (GLuint*)stack->Push(indices_size);

    glGenBuffers(1, &m_Id);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Id);
    m_Count = 0;
    m_MaxCount = max_index_count;
    m_Indices = indices;
}

uint32_t *GlIndexBuffer::GetData() {
    return m_Indices;
}

uint32_t GlIndexBuffer::GetCount() {
    return m_Count;
}

void GlIndexBuffer::Reset() {
    m_Count = 0;
    m_CurrentIndex = 0;
}

void GlIndexBuffer::PushRectangle() {
    uint32_t current_index = m_CurrentIndex;
    int count = m_Count;
    m_Indices[count++] = current_index + 0;
    m_Indices[count++] = current_index + 1;
    m_Indices[count++] = current_index + 2;
    m_Indices[count++] = current_index + 0;
    m_Indices[count++] = current_index + 2;
    m_Indices[count++] = current_index + 3;

    m_Count = count;
    m_CurrentIndex += 4;
    assert(m_Count < m_MaxCount);
}

void GlIndexBuffer::TransferData() {
    size_t size = m_Count * sizeof(uint32_t);
    printf("transfering %d/%d indices\n", m_Count, m_MaxCount);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, m_Indices, GL_DYNAMIC_DRAW);
}

