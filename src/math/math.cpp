#include <math/math.h>

V2::V2(float x, float y) {
    this->x = x;
    this->y = y;
}

V3::V3(float x, float y, float z) {
    this->x = x;
    this->y = y;
    this->z = z;
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
