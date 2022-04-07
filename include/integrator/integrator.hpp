#pragma once 
#include <memory>
#include <string>
#include "sampling/sampler.hpp"
#include "math/vec3.h"
#include "core/ray.h"
#include "scene/scene.h"
#include "sampling/sampler.hpp"

class Integrator {
public:
    virtual Vec3 integrate(const Ray& ray, const Scene& scene, std::shared_ptr<Sampler>& sampler) const = 0;
    virtual std::string getIntegratorType() const = 0;
};