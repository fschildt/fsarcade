#pragma once

#include <renderer/Renderer.hpp>

struct GlVertex {
    V3 pos;
    V3 color;
};

class GlVertexBuffer {
public:
    void Init();
    void Reset();

public:
    float *GetData();
    uint32_t GetCount();

    void PushRectangle(V3 pos, V2 dim, V3 color);
    void TransferData();

private:
    uint32_t m_Id;
    std::vector<GlVertex> m_Vertices;
};

