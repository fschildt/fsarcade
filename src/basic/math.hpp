#pragma once

#include <basic/defs.hpp>

union V2ST {
    V2ST() = default;
    V2ST(size_t x, size_t y);
    bool operator==(V2ST &b);
    struct {
        size_t x;
        size_t y;
    };
};

union V2F32 {
    V2F32() = default;
    V2F32(float x, float y);
    V2F32 operator/(float scalar);
    V2F32 operator*(float scalar);
    struct {
        float x;
        float y;
    };
};

union V3F32 {
    V3F32() = default;
    V3F32(float x, float y, float z);
    V3F32 operator/(float scalar);
    V3F32 operator*(float scalar);
    struct {
        float r;
        float g;
        float b;
    };
    struct {
        float x;
        float y;
        float z;
    };
};

union V4F32 {
    V4F32() = default;
    V4F32 (float x, float y, float z, float w);
    V4F32 operator/(float scalar);
    V4F32 operator*(float scalar);
    struct {
        float r;
        float g;
        float b;
        float a;
    };
    struct {
        float x;
        float y;
        float z;
        float w;
    };
};


