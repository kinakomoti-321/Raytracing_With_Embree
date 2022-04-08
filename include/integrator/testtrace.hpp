#pragma once
#include "integrator/integrator.hpp"
#include "core/ray.h"
#include "scene/scene.h"
#include "sampling/sampler.hpp"
#include "math/vec3.h"
#include "core/intersectinfo.h"
#include "core/constant.hpp"

class TestTrace :public Integrator {

public:
    Vec3 integrate(const Ray& ray, const Scene& scene, std::shared_ptr<Sampler>& sampler) const {
        IntersectInfo info;
        if (!scene.Intersection(ray, info)) {
            return Vec3(0);
        }

        auto brdf = scene.faceMaterial(info.FaceID);
        Vec3 wo;
        Vec3 wi;
        float pdf;

        return brdf->samplingBSDF(wo, wi, pdf, sampler) * PI;
    };
    std::string getIntegratorType() const {
        return "NormalChecker";
    };
};