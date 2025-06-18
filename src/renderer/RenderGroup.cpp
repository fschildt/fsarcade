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

float RenderGroup::GetScale() {
    float screen_width = static_cast<float>(m_ScreenWidth);
    float screen_height = static_cast<float>(m_ScreenHeight);
    float xunits = screen_width / m_CameraWidth;
    float yunits = screen_height / m_CameraHeight;
    float scale = std::min(xunits, yunits);
    return scale;
}

V2F32 RenderGroup::ViewPosToScreenPos(V2F32 view_pos) {
    float scale = GetScale();
    float screen_width = static_cast<float>(m_ScreenWidth);
    float screen_height = static_cast<float>(m_ScreenHeight);
    float viewport_width = m_CameraWidth * scale;
    float viewport_height = m_CameraHeight * scale;
    float viewport_x0 = (screen_width - viewport_width) / 2;
    float viewport_y0 = (screen_height - viewport_height) / 2;

    V2F32 result;
    result.x = viewport_x0 + view_pos.x * scale;
    result.y = screen_height - (viewport_y0 + view_pos.y * scale);

    return result;
}

V2F32 RenderGroup::ViewSizeToScreenSize(V2F32 view_size) {
    float scale = GetScale();

    V2F32 result;
    result.x = view_size.x * scale;
    result.y = view_size.y * scale;

    return result;
}

ImVec2 RenderGroup::ViewPosToScreenPosImGui(V2F32 view_pos) {
    V2F32 screen_pos = ViewPosToScreenPos(view_pos);
    ImVec2 result = {screen_pos.x, screen_pos.y};
    return result;
}

ImVec2 RenderGroup::ViewSizeToScreenSizeImGui(V2F32 view_size) {
    V2F32 screen_size = ViewSizeToScreenSize(view_size);
    ImVec2 result = {screen_size.x, screen_size.y};
    return result;
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

