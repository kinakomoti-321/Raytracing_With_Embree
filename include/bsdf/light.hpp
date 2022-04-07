#pragma once
#include "math/vec3.h"
class Light {
private:
    Vec3 radiance;
public:
    Light(const Vec3& radiance) :radiance(radiance) {}

    Vec3 le() const {
        return radiance;
    }
};