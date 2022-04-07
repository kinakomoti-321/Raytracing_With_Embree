#pragma once
#include <string>
#include "core/ray.h"
#include "math/vec3.h"

class Camera {
public:
    virtual Ray getCameraRay(float u, float v) const = 0;
    virtual std::string getCameraType() const = 0;
};