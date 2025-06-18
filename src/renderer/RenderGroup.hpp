#pragma once

#include <basic/defs.hpp>
#include <basic/math.hpp>
#include <vector>
#include <imgui.h>

enum REntityType : int32_t {
    REntityType_Rectangle,
    REntityType_Bitmap,
};

struct REntity_Rectangle {
    REntityType type;
    V3F32 pos;
    V2F32 dim;
    V3F32 color;
};

struct REntity_Bitmap {
    REntityType type;
    V3F32 pos;
    int32_t width;
    int32_t height;
    void *bitmap;
};

union REntity {
    REntityType type;
    REntity_Rectangle rect;
    REntity_Bitmap bitmap;
};

struct RSortEntry {
    RSortEntry(float z, size_t entity_index);
    float z;
    size_t entity_index;
};


class RenderGroup {
public:
    RenderGroup();
    void Clear(V3F32 color);
    void Reset();

    void SetCameraSize(float width, float height);
    V2F32 ViewPosToScreenPos(V2F32 view_pos);
    V2F32 ViewSizeToScreenSize(V2F32 view_size);
    ImVec2 ViewPosToScreenPosImGui(V2F32 view_pos);
    ImVec2 ViewSizeToScreenSizeImGui(V2F32 view_size);
    float GetScale();


public:
    void PushRectangle(V3F32 pos, V2F32 dim, V3F32 color);
    void PushBitmap(V3F32 pos, int32_t width, int32_t height, void *bitmap);


private:
    void Sort();


public:
    int32_t m_ScreenWidth;
    int32_t m_ScreenHeight;


private:
    friend class GlRenderer;


    float m_CameraWidth;
    float m_CameraHeight;


    V3F32 m_ClearColor;

    std::vector<REntity> m_REntities;
    std::vector<RSortEntry> m_RSortEntries;
};


