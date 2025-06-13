#include <renderer/RenderGroup.hpp>
#include <algorithm>

RSortEntry::RSortEntry(float z, size_t entity_index)
        : z(z), entity_index(entity_index) {
}

RenderGroup::RenderGroup() {
    m_REntities.reserve(1024);
    m_RSortEntries.reserve(1024);
}

void RenderGroup::Reset() {
    m_CameraWidth = 0;
    m_CameraHeight = 0;
    m_REntities.clear();
    m_RSortEntries.clear();
    m_REntities.reserve(1024);
    m_RSortEntries.reserve(1024);
}

void RenderGroup::SetCameraSize(float width, float height) {
    m_CameraWidth = width;
    m_CameraHeight = height;
}

void RenderGroup::Clear(V3F32 color) {
    m_ClearColor = color;
}

void RenderGroup::PushRectangle(V3F32 pos, V2F32 dim, V3F32 color) {
    m_REntities.emplace_back(REntity{.rect{REntityType_Rectangle, pos, dim, color}});
    m_RSortEntries.emplace_back(pos.z, m_REntities.size()-1);
}

void RenderGroup::Sort() {
    std::sort(m_RSortEntries.begin(), m_RSortEntries.end(),
              [](const RSortEntry& e1, const RSortEntry& e2) {
                  return e1.z < e2.z;
              });
}

