#pragma once 
#include <memory>
#include <cmath>
#include "sampling/sampler.hpp"
#include "math/vec3.h"
#include "core/ray.h"
#include "scene/scene.h"
#include "sampling/sampler.hpp"
#include "integrator.hpp"
#include <memory>
#include "bsdf/lambert.hpp"
#include "bsdf/bsdf.hpp"
class PathTracer :public Integrator {
public:
    Vec3 integrate(const Ray& ray, const Scene& scene, std::shared_ptr<Sampler>& sampler)const {
        const int MaxDepth = 10;
        float p = 1.0f;
        Vec3 throughput(1.0);
        Vec3 LTE(0);
        Ray next_ray = ray;
        for (int i = 0; i < MaxDepth; i++) {
            p = std::min(std::max(std::max(throughput[0], throughput[1]), throughput[2]), 1.0f);
            if (sampler->getSample() > p) {
                break;
            }

            throughput /= p;

            IntersectInfo info;
            if (!scene.Intersection(next_ray, info)) {
                LTE = throughput * scene.getSkyLe(next_ray.direction);
                break;
            }

            if (scene.faceHasLight(info.FaceID)) {
                LTE = throughput * scene.faceLight(info.FaceID)->le();
                break;
            }
            Vec3 t, b;
            tangentSpaceBasis(info.normal, t, b);
            Vec3 wo = worldtoLocal(-next_ray.direction, t, info.normal, b);
            Vec3 wi;
            float pdf;
            Vec3 bsdf;

            // BSDF計算
            auto inbsdf = scene.faceMaterial(info.FaceID);
            inbsdf->textureUVSet(info.texcoord);
            bsdf = inbsdf->samplingBSDF(wo, wi, pdf, sampler);

            const Vec3 next_direction = localToWorld(wi, t, info.normal, b);
            const float cosine = std::abs(dot(info.normal, next_direction));
            throughput *= bsdf * cosine / pdf;
            next_ray = Ray(info.position, next_direction);
        }
        return LTE;
    }
    std::string getIntegratorType()const {
        return "PathTrace";
    }
};