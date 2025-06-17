#pragma once

#include <basic/defs.hpp>

struct V2ST {
    size_t x;
    size_t y;

    V2ST() = default;
    V2ST(size_t x, size_t y);
    bool operator==(V2ST &b);
    bool operator==(const V2ST& other) const {
        return x == other.x && y == other.y;
    }
};

struct V2F32 {
    float x;
    float y;

    V2F32() = default;
    V2F32(float x, float y);
    V2F32 operator/(float scalar);
    V2F32 operator*(float scalar);
    V2F32 operator+(V2F32 other);
};

struct V3F32 {
    float x;
    float y;
    float z;

    V3F32() = default;
    V3F32(float x, float y, float z);
    V3F32 operator/(float scalar);
    V3F32 operator*(float scalar);
};

struct V4F32 {
    float x;
    float y;
    float z;
    float w;

    V4F32() = default;
    V4F32 (float x, float y, float z, float w);
    V4F32 operator/(float scalar);
    V4F32 operator*(float scalar);
};

struct V2I32 {
    float x;
    float y;

    V2I32() = default;
    V2I32 (float x, float y);
};
