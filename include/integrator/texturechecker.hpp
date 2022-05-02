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
        // if (norm(material->getBaseColor(info.texcoord)) < 0.001f) return Vec3(1, 0, 0);
        return material->getBaseColor(info.texcoord);
    };
    std::string getIntegratorType() const {
        return "UVChecker";
    };
};