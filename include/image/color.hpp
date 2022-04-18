#pragma once 
#include "math/vec3.h"

float YfromRGB(const Vec3& RGB) {
    float xy = 0.2126;
    float yy = 0.7152;
    float zy = 0.0772;

    return xy * RGB[0] + yy * RGB[1] + zy * RGB[2];
}