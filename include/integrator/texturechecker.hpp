#pragma once
#include "integrator/integrator.hpp"
#include "core/ray.h"
#include "scene/scene.h"
#include "sampling/sampler.hpp"
#include "math/vec3.h"
#include "core/intersectinfo.h"

class TextureChecker :public Integrator {

public:
    Vec3 integrate(const Ray& ray, const Scene& scene, std::shared_ptr<Sampler>& sampler) const {
        IntersectInfo info;
        if (!scene.Intersection(ray, info)) return Vec3(0);
        auto material = scene.faceMaterial(info.FaceID);
        auto bsdf = material->getBSDF(info.texcoord);
        Vec3 wo;
        Vec3 wi;
        float pdf;
        return bsdf->samplingBSDF(wo, wi, pdf, sampler);
    };
    std::string getIntegratorType() const {
        return "UVChecker";
    };
};