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

ImVec2 RenderGroup::ViewPosToImguiPos(V2F32 view_pos) {
    float scale = GetScale();
    float viewport_width = m_CameraWidth * scale;
    float viewport_height = m_CameraHeight * scale;
    float viewport_x0 = (m_ScreenWidth - viewport_width) / 2;
    float viewport_y0 = (m_ScreenHeight - viewport_height) / 2;

    ImVec2 result = {
        viewport_x0 + view_pos.x * scale,
        m_ScreenHeight - (viewport_y0 + view_pos.y * scale)
    };

    return result;
}

ImVec2 RenderGroup::ViewDimToImguiDim(V2F32 view_dim) {
    float scale = GetScale();

    ImVec2 result = {
        view_dim.x * scale,
        view_dim.y * scale
    };

    return result;
}

float RenderGroup::GetScale() {
    float xunits = m_ScreenWidth / m_CameraWidth;
    float yunits = m_ScreenHeight / m_CameraHeight;
    float scale = std::min(xunits, yunits);
    return scale;
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

