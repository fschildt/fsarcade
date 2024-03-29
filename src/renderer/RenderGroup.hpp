#pragma once

#include <basic/defs.hpp>
#include <basic/math.hpp>
#include <vector>

enum REntityType : int32_t {
    REntityType_Clear,
    REntityType_Rectangle,
    REntityType_Bitmap,
};

struct REntity_Clear {
    REntityType type;
    V3 color;
};

struct REntity_Rectangle {
    REntityType type;
    V3 pos;
    V2 dim;
    V3 color;
};

struct REntity_Bitmap {
    REntityType type;
    V3 pos;
    int32_t width;
    int32_t height;
    void *bitmap;
};

union REntity {
    REntityType type;
    REntity_Clear clear;
    REntity_Rectangle rect;
    REntity_Bitmap bitmap;
};

struct RSortEntry {
    float z;
    size_t entity_index;
};


class RenderGroup {
public:
    RenderGroup();
    void SetSize(float xmax, float ymax);
    void Reset();
    void Sort();

public:
    void PushClear(V3 color);
    void PushRectangle(V3 pos, V2 dim, V3 color);
    void PushBitmap(V3 pos, int32_t width, int32_t height, void *bitmap);

private:
    friend class GlRenderer;

    float m_XMax;
    float m_YMax;

    std::vector<REntity> m_REntities;
    std::vector<std::pair<float, size_t>> m_RSortEntries;
};

