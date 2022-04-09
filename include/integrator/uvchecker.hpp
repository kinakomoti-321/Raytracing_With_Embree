#pragma once
#include "integrator/integrator.hpp"
#include "core/ray.h"
#include "scene/scene.h"
#include "sampling/sampler.hpp"
#include "math/vec3.h"
#include "core/intersectinfo.h"

class UVChecker :public Integrator {

public:
    Vec3 integrate(const Ray& ray, const Scene& scene, std::shared_ptr<Sampler>& sampler) const {
        IntersectInfo info;
        scene.Intersection(ray, info);
        return Vec3(info.texcoord[0], info.texcoord[1], 0);
    };
    std::string getIntegratorType() const {
        return "UVChecker";
    };
};