#include <renderer/RenderGroup.h>

#define PUSH_RENDER_ENTRY(stack, type)

RenderGroup::RenderGroup(uint8_t *memory, size_t memory_size) {
    Stack stack(memory, memory_size);

    int32_t max_render_entry_count = 1024;
    size_t size_render_entries = max_render_entry_count * sizeof(RenderEntry);
    size_t size_sort_entries = max_render_entry_count * sizeof(RenderSortEntry);

    mRenderEntryCount = 0;
    mMaxRenderEntryCount = max_render_entry_count;
    mRenderEntries = (RenderEntry*)stack.Push(size_render_entries);
    mRenderSortEntries = (RenderSortEntry*)stack.Push(size_sort_entries);
}

void RenderGroup::Reset() {
    mRenderEntryCount = 0;
    m_XMax = 0;
    m_YMax = 0;
}

void RenderGroup::SetSize(float xmax, float ymax) {
    m_XMax = xmax;
    m_YMax = ymax;
}

void RenderGroup::PushClear(V3 color) {
    RenderEntry_Clear *clear = (RenderEntry_Clear*)PushRenderEntry();
    clear->type = RenderEntryType_Clear;
    clear->color = color;
}

void RenderGroup::PushRectangle(V3 pos, V2 dim, V3 color) {
    RenderEntry_Rectangle *rect = (RenderEntry_Rectangle*)PushRenderEntry();
    rect->type = RenderEntryType_Rectangle;
    rect->pos = pos;
    rect->dim = dim;
    rect->color = color;
}

RenderEntry *RenderGroup::PushRenderEntry() {
    RenderEntry *entry = &mRenderEntries[mRenderEntryCount++];
    return entry;
}

void RenderGroup::Sort() {
    int render_entry_count = mRenderEntryCount;

    // make sort entry array
    for (int32_t i = 0; i < render_entry_count; i++) {
        RenderEntry *render_entry = mRenderEntries + i;
        if (render_entry->type == RenderEntryType_Clear) {
            mRenderSortEntries[i].z = -1;
        } else if (render_entry->type == RenderEntryType_Rectangle) {
            mRenderSortEntries[i].z = render_entry->rect.pos.z;
        }
        mRenderSortEntries[i].value = render_entry;
    }

    // sort array
    // Todo: use an efficient sorting algorithm and not bubblesort
    RenderSortEntry *sort_entries = mRenderSortEntries;
    for (int32_t i = 0; i < render_entry_count-1; i++) {
        for (int32_t j = 0; j < render_entry_count-i-1; j++) {
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

