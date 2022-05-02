
#pragma once 
#include <memory>
#include <cmath>
#include "sampling/sampler.hpp"
#include "math/vec3.h"
#include "math/hash.h"
#include "core/ray.h"
#include "scene/scene.h"
#include "sampling/sampler.hpp"
#include "integrator.hpp"
#include <memory>
#include "bsdf/lambert.hpp"
#include "bsdf/bsdf.hpp"
class DebugPathTracer :public Integrator {
public:
    Vec3 integrate(const Ray& ray, const Scene& scene, std::shared_ptr<Sampler>& sampler)const {
        const int MaxDepth = 10;
        float p = 0.99f;
        Vec3 throughput(1.0);
        Vec3 LTE(0);
        Ray next_ray = ray;
        float depth = 0;
        for (int i = 0; i < MaxDepth; i++) {
            // p = std::min(std::max(std::max(throughput[0], throughput[1]), throughput[2]), 1.0f);
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
                // std::cout << LTE << std::endl;
                break;
            }
            Vec3 t, b;
            tangentSpaceBasis(info.normal, t, b);
            Vec3 wo = worldtoLocal(-next_ray.direction, t, info.normal, b);
            Vec3 wi;
            float pdf;
            Vec3 bsdf;

            // BSDF計算
            auto material = scene.faceMaterial(info.FaceID);
            auto inbsdf = material->getBSDF(info.texcoord);


            bsdf = inbsdf->samplingBSDF(wo, wi, pdf, sampler);
            // bsdf = inbsdf->evaluateBSDF(wo, wi);
            // pdf = 1 / PI2;

            const Vec3 next_direction = localToWorld(wi, t, info.normal, b);
            const float cosine = std::abs(dot(info.normal, next_direction));
            throughput *= bsdf * cosine / pdf;
            throughput = Vec3(std::abs(throughput[0]), std::abs(throughput[1]), std::abs(throughput[2]));
            if (isnan(throughput[0])) {
                std::cout << "MaterialCol " << material->getBaseColor(info.texcoord) << std::endl;
                std::cout << "Normal " << info.normal << std::endl;

                std::cout << "NextDirection " << next_direction << std::endl;
                std::cout << "distance " << info.distance << std::endl;
                std::cout << "bsdf " << bsdf << std::endl;
                std::cout << "cosine " << cosine << std::endl;
                std::cout << "pdf " << pdf << std::endl;
                std::cout << std::endl;
            }
            next_ray = Ray(info.position, next_direction);
            depth++;
        }
        Vec3 col = Vec3(YfromRGB(LTE));
        if (isnan(LTE[0])) col = Vec3(1, 0, 0);
        return LTE;
    }
    std::string getIntegratorType()const {
        return "PathTrace";
    }
};