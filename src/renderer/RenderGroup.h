#pragma once

#include <fsarcade_defs.h>
#include <math/math.h>
#include <util/Stack.h>

enum RenderEntryType : int32_t {
    RenderEntryType_Clear,
    RenderEntryType_Rectangle,
    RenderEntryType_Text,
};

struct RenderEntry_Clear {
    RenderEntryType type;
    V3 color;
};

struct RenderEntry_Rectangle {
    RenderEntryType type;
    V3 pos;
    V2 dim;
    V3 color;
};

struct RenderEntry_Text {
    RenderEntryType type;
    V3 pos;
    char *text;
};

union RenderEntry {
    RenderEntryType type;
    RenderEntry_Clear clear;
    RenderEntry_Rectangle rect;
    RenderEntry_Rectangle text;
};

struct RenderSortEntry {
    float z; // key
    RenderEntry *value;
};


class RenderGroup {
public:
    RenderGroup(uint8_t *memory, size_t memory_size);
    void SetSize(float xmax, float ymax);
    void Reset();
    void Sort();

public:
    void PushClear(V3 color);
    void PushRectangle(V3 pos, V2 dim, V3 color);
    void PushText(V3 pos, char *text);

private:
    RenderEntry *PushRenderEntry();

private:
    friend class GlRenderer;

    float m_XMax;
    float m_YMax;

    int32_t mMaxRenderEntryCount;
    int32_t mRenderEntryCount;
    RenderEntry *mRenderEntries;
    RenderSortEntry *mRenderSortEntries;
};

