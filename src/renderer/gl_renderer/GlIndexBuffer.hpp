#pragma once

#include <renderer/Renderer.hpp>

class GlIndexBuffer {
public:
    GlIndexBuffer() = default;
    void Init();
    void Reset();

public:
    uint32_t GetCount();
    uint32_t *GetData();

    void PushRectangle();
    void TransferData();

private:
    uint32_t m_Id;
    uint32_t m_CurrentIndex;
    std::vector<uint32_t> m_Indices;
};

