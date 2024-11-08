#include <renderer/RenderGroup.hpp>
#include <algorithm>

#define PUSH_RENDER_ENTRY(stack, type)

RenderGroup::RenderGroup() {
    m_REntities.reserve(1024);
    m_RSortEntries.reserve(1024);
}

void RenderGroup::Reset() {
    m_XMax = 0;
    m_YMax = 0;
    m_REntities.clear();
    m_RSortEntries.clear();
}

void RenderGroup::SetSize(float xmax, float ymax) {
    m_XMax = xmax;
    m_YMax = ymax;
}

void RenderGroup::PushClear(V3 color) {
    REntity entity;
    entity.clear.type = REntityType_Clear;
    entity.clear.color = color;
    m_REntities.push_back(entity);
    m_RSortEntries.emplace_back(-1, m_REntities.size()-1);
}

void RenderGroup::PushRectangle(V3 pos, V2 dim, V3 color) {
    REntity entity;
    entity.rect.type = REntityType_Rectangle;
    entity.rect.pos = pos;
    entity.rect.dim = dim;
    entity.rect.color = color;
    m_REntities.push_back(entity);
    m_RSortEntries.emplace_back(pos.z, m_REntities.size()-1);
}

void RenderGroup::Sort() {
    sort(m_RSortEntries.begin(), m_RSortEntries.end(), [](std::pair<float, size_t>& p1, std::pair<float, size_t>& p2){return p1.first < p2.first;});
}

