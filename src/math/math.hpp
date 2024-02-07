#pragma once

#include <stdint.h>

union V2 {
    V2() = default;
    V2(float x, float y);
    V2 operator/(float scalar);
    V2 operator*(float scalar);
    struct {
        float x;
        float y;
    };
};

union V3 {
    V3() = default;
    V3(float x, float y, float z);
    V3 operator/(float scalar);
    V3 operator*(float scalar);
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

union V4 {
    V4() = default;
    V4 (float x, float y, float z, float w);
    V4 operator/(float scalar);
    V4 operator*(float scalar);
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


