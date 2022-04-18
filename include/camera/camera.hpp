#pragma once
#include <string>
#include "core/ray.h"
#include "math/vec3.h"
#include "sampling/sampler.hpp"
#include <memory>

class Camera {
public:
    virtual Ray getCameraRay(float u, float v,
        const std::shared_ptr<Sampler>& sampler, float& weight) const = 0;
    virtual std::string getCameraType() const = 0;
};