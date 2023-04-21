#pragma once

#include <stdint.h>
#include <math/math.h>
#include <util/Stack.h>

enum RenderEntryType : int32_t {
    RenderEntryType_Clear,
    RenderEntryType_Rectangle,
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

struct RenderSortEntry {
    float z; // key
    void *render_entry; // value
};

typedef RenderEntryType* RenderEntry;

class RenderGroup {
public:
    RenderGroup(Stack *stack);
    void Reset();
    void Sort();

public:
    void SetScale(float xmax, float ymax);
    void PushClear(V3 color);
    void PushRectangle(V3 pos, V2 dim, V3 color);

private:
    template<typename T> T* PushRenderEntry(float z);

private:
    friend class GlRenderer;

    float m_XMax;
    float m_YMax;

    Stack m_RenderEntries;
    Stack m_SortEntries;
};

