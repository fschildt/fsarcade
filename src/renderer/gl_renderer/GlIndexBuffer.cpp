#include <renderer/gl_renderer/GlIndexBuffer.hpp>

#include <GL/glew.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

void GlIndexBuffer::Init() {
    m_CurrentIndex = 0;
    m_Indices.reserve(16384);
    glGenBuffers(1, &m_Id);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Id);
}

uint32_t *GlIndexBuffer::GetData() {
    return m_Indices.data();
}

uint32_t GlIndexBuffer::GetCount() {
    uint32_t count = static_cast<uint32_t>(m_Indices.size());
    return count;
}

void GlIndexBuffer::Reset() {
    m_CurrentIndex = 0;
    m_Indices.clear();
}

void GlIndexBuffer::PushRectangle() {
    uint32_t current_index = m_CurrentIndex;
    m_Indices.push_back(current_index + 0);
    m_Indices.push_back(current_index + 1);
    m_Indices.push_back(current_index + 2);
    m_Indices.push_back(current_index + 0);
    m_Indices.push_back(current_index + 2);
    m_Indices.push_back(current_index + 3);
    m_CurrentIndex += 4;
}

void GlIndexBuffer::TransferData() {
    size_t size = m_Indices.size() * sizeof(uint32_t);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, m_Indices.data(), GL_STATIC_DRAW);
}

