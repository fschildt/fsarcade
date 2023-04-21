#include <renderer/RenderGroup.h>

#define PUSH_RENDER_ENTRY(stack, type)

RenderGroup::RenderGroup(Stack *stack) {
    // Todo: find optimal ratio
    size_t size = 1024*1024;
    float ratio = 0.7;

    size_t size_render_entries = size * ratio;
    size_t size_sort_entries = size - size_render_entries;

    stack->MakeSubstack(&m_RenderEntries, size_render_entries);
    stack->MakeSubstack(&m_SortEntries, size_sort_entries);
}

void RenderGroup::SetScale(float xmax, float ymax) {
    m_XMax = xmax;
    m_YMax = ymax;
}

void RenderGroup::Reset() {
    m_RenderEntries.Reset();
    m_SortEntries.Reset();
    m_XMax = 0;
    m_YMax = 0;
}

void RenderGroup::PushClear(V3 color) {
    RenderEntry_Clear *clear = PushRenderEntry<RenderEntry_Clear>(-1.f);
    clear->type = RenderEntryType_Clear;
    clear->color = color;
}

void RenderGroup::PushRectangle(V3 pos, V2 dim, V3 color) {
    RenderEntry_Rectangle *rect = PushRenderEntry<RenderEntry_Rectangle>(pos.z);
    rect->type = RenderEntryType_Rectangle;
    rect->pos = pos;
    rect->dim = dim;
    rect->color = color;
}

template<typename T> T*
RenderGroup::PushRenderEntry(float z) {
    size_t size = sizeof(T);
    T *render_entry = (T*)m_RenderEntries.Push(size);

    RenderSortEntry *sort_entry = (RenderSortEntry*)m_SortEntries.Push(sizeof(RenderSortEntry));
    sort_entry->z = z;
    sort_entry->render_entry = render_entry;

    return render_entry;
}

void RenderGroup::Sort() {
    int32_t count = m_SortEntries.m_SizeUsed / sizeof(RenderSortEntry);
    RenderSortEntry *sort_entries = (RenderSortEntry*)m_SortEntries.m_Memory;

    // Todo: use an efficient sorting algorithm and not bubblesort
    for (int32_t i = 0; i < count-1; i++) {
        for (int32_t j = 0; j < count-i-1; j++) {
            RenderSortEntry *entry0 = &sort_entries[j+0];
            RenderSortEntry *entry1 = &sort_entries[j+1];
            if (entry0->z > entry1->z) {
                RenderSortEntry tmp = *entry0;
                *entry0 = *entry1;
                *entry1 = tmp;
            }
        }
    }
}

