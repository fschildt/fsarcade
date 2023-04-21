#pragma once

#include <renderer/Renderer.h>

class GlIndexBuffer {
public:
    void Init(Stack *stack);
    void Reset();

public:
    uint32_t GetCount();
    uint32_t *GetData();

    void PushRectangle();
    void TransferData();

private:
    uint32_t m_Id;
    int32_t m_Count;
    int32_t m_MaxCount;
    uint32_t m_CurrentIndex;
    uint32_t *m_Indices;
};

