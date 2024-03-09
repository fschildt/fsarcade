#include <basic/math.hpp>

V2::V2(float x, float y) {
    this->x = x;
    this->y = y;
}

V2 V2::operator/(float scalar) {
    V2 result = {};
    result.x = this->x / scalar;
    result.y = this->y / scalar;
    return result;
}

V2 V2::operator*(float scalar) {
    V2 result = {};
    result.x = this->x * scalar;
    result.y = this->y * scalar;
    return result;
}


V3::V3(float x, float y, float z) {
    this->x = x;
    this->y = y;
    this->z = z;
}

V3 V3::operator/(float scalar) {
    V3 result = {};
    result.x = this->x / scalar;
    result.y = this->y / scalar;
    result.z = this->z / scalar;
    return result;
}

V3 V3::operator*(float scalar) {
    V3 result = {};
    result.x = this->x * scalar;
    result.y = this->y * scalar;
    result.z = this->z * scalar;
    return result;
}


V4::V4(float x, float y, float z, float w) {
    this->x = x;
    this->y = y;
    this->z = z;
    this->z = z;
}

V4 V4::operator/(float scalar) {
    V4 result = {};
    result.x = this->x / scalar;
    result.y = this->y / scalar;
    result.z = this->z / scalar;
    result.w = this->w / scalar;
    return result;
}

V4 V4::operator*(float scalar) {
    V4 result = {};
    result.x = this->x * scalar;
    result.y = this->y * scalar;
    result.z = this->z * scalar;
    result.w = this->z * scalar;
    return result;
}

