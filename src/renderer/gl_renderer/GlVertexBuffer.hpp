#pragma once

#include <renderer/Renderer.hpp>

struct GlVertex {
    V3F32 pos;
    V3F32 color;
};

class GlVertexBuffer {
public:
    GlVertexBuffer() = default;
    void Init();
    void Reset();

public:
    float *GetData();
    uint32_t GetCount();

    void PushRectangle(V3F32 pos, V2F32 dim, V3F32 color);
    void TransferData();

private:
    uint32_t m_Id;
    std::vector<GlVertex> m_Vertices;
};

