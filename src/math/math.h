#pragma once

#include <stdint.h>

union V3 {
    V3() {}
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

