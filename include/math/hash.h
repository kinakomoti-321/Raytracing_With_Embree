#pragma once 
#include "vec3.h"
#include "vec2.h"
#include <cmath>

inline float Hash1Dto1D(float u) {
    return std::floor(std::sin(u * 103.0f) * 13042.0f);
}
inline Vec2 Hash1Dto2D(float u) {
    return Vec2(std::floor(std::sin(u * 103.0f) * 13042.0f), std::floor(std::sin(u * 49.0f) * 12042.0f));
}
inline Vec3 Hash1Dto3D(float u) {
    return Vec3(std::floor(std::sin(u * 103.0f) * 13042.0f), std::floor(std::sin(u * 49.0f) * 12042.0f), std::floor(std::sin(u * 134.0f) * 49231.0f));
}