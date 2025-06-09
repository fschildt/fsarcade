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
    m_XMax = 0;
    m_YMax = 0;
    m_REntities.clear();
    m_RSortEntries.clear();
    m_REntities.reserve(1024);
    m_RSortEntries.reserve(1024);
}

void RenderGroup::SetSize(float xmax, float ymax) {
    m_XMax = xmax;
    m_YMax = ymax;
}

void RenderGroup::PushClear(V3 color) {
    m_REntities.emplace_back(REntity{.clear{.type = REntityType_Clear, .color = color}});
    m_RSortEntries.emplace_back(-1, m_REntities.size()-1);
}

void RenderGroup::PushRectangle(V3 pos, V2 dim, V3 color) {
    m_REntities.emplace_back(REntity{.rect{REntityType_Rectangle, pos, dim, color}});
    m_RSortEntries.emplace_back(pos.z, m_REntities.size()-1);
}

void RenderGroup::Sort() {
    std::sort(m_RSortEntries.begin(), m_RSortEntries.end(),
              [](const RSortEntry& e1, const RSortEntry& e2) {
                  return e1.z < e2.z;
              });
}

