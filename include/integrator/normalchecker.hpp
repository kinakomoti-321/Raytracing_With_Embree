#pragma once
#include "integrator/integrator.hpp"
#include "core/ray.h"
#include "scene/scene.h"
#include "sampling/sampler.hpp"
#include "math/vec3.h"
#include "core/intersectinfo.h"

class NoramlChecker :public Integrator {

public:
    Vec3 integrate(const Ray& ray, const Scene& scene, std::shared_ptr<Sampler>& sampler) const {
        IntersectInfo info;
        scene.Intersection(ray, info);
        return 0.5f * info.normal + Vec3(0.5f);
    };
    std::string getIntegratorType() const {
        return "NormalChecker";
    };
};