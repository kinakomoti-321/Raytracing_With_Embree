#pragma once 

#include "math/vec3.h"

class Sky {
private:
    Vec3 LE;
public:
    Sky() {
        LE = Vec3(0.0);
    }
    Sky(const Vec3& LE) : LE(LE) { }

    Vec3 Le(Vec3 rayDir)const {
        return LE;
    }
};