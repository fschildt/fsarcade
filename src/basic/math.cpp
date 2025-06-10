#include <basic/math.hpp>


V2ST::V2ST(size_t x, size_t y) : x(x), y(y) {
}

bool V2ST::operator==(V2ST &b) {
    bool result = this->x == b.x && this->y == b.y;
    return result;
}


V2F32::V2F32(float x, float y) {
    this->x = x;
    this->y = y;
}

V2F32 V2F32::operator/(float scalar) {
    V2F32 result = {};
    result.x = this->x / scalar;
    result.y = this->y / scalar;
    return result;
}

V2F32 V2F32::operator*(float scalar) {
    V2F32 result = {};
    result.x = this->x * scalar;
    result.y = this->y * scalar;
    return result;
}

V2F32 V2F32::operator+(V2F32 other) {
    V2F32 result = {};
    result.x = this->x + other.x;
    result.y = this->y + other.y;
    return result;
}


V3F32::V3F32(float x, float y, float z) {
    this->x = x;
    this->y = y;
    this->z = z;
}

V3F32 V3F32::operator/(float scalar) {
    V3F32 result = {};
    result.x = this->x / scalar;
    result.y = this->y / scalar;
    result.z = this->z / scalar;
    return result;
}

V3F32 V3F32::operator*(float scalar) {
    V3F32 result = {};
    result.x = this->x * scalar;
    result.y = this->y * scalar;
    result.z = this->z * scalar;
    return result;
}


V4F32::V4F32(float x, float y, float z, float w) {
    this->x = x;
    this->y = y;
    this->z = z;
    this->z = z;
}

V4F32 V4F32::operator/(float scalar) {
    V4F32 result = {};
    result.x = this->x / scalar;
    result.y = this->y / scalar;
    result.z = this->z / scalar;
    result.w = this->w / scalar;
    return result;
}

V4F32 V4F32::operator*(float scalar) {
    V4F32 result = {};
    result.x = this->x * scalar;
    result.y = this->y * scalar;
    result.z = this->z * scalar;
    result.w = this->z * scalar;
    return result;
}

V2I32::V2I32(float x, float y) : x(x), y(y) {
}
