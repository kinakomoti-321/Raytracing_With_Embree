#pragma once 
#include <memory>
#include <cmath>
#include "sampling/sampler.hpp"
#include "math/vec3.h"
#include "core/ray.h"
#include "scene/scene.h"
#include "sampling/sampler.hpp"
#include "integrator.hpp"
class PathTracer :public Integrator {
public:
    Vec3 integrate(const Ray& ray, const Scene& scene, std::shared_ptr<Sampler>& sampler)const {
        int MAXDEPTH = 100;
        float rp = 1.0;
        Vec3 LTE = Vec3(0);
        Vec3 throughput = Vec3(1.0);
        Ray next_ray = ray;
        for (int i = 0; i < MAXDEPTH; i++) {

            //ロシアンルーレット
            rp = std::min(std::max(std::max(throughput[0], throughput[1]), throughput[2]), 0.99f);
            if (rp < sampler->getSample()) break;
            throughput /= rp;

            IntersectInfo info;
            if (!scene.Intersection(ray, info)) {
                LTE = throughput * Vec3(1.0);
                break;
            }

            if (scene.faceHasLight(info.FaceID)) {
                LTE = throughput * scene.faceLight(info.FaceID)->le();
                break;
            }

            auto faceBSDF = scene.faceMaterial(info.FaceID);
            Vec3 t, b;
            tangentSpaceBasis(info.normal, t, b);
            Vec3 wo = worldtoLocal(-next_ray.direction, t, info.normal, b);
            Vec3 wi;
            float pdf;
            Vec3 bsdf;

            bsdf = faceBSDF->samplingBSDF(wo, wi, pdf, sampler);

            const Vec3 next_direction = localToWorld(wi, t, info.normal, b);
            const float cosine = std::abs(dot(info.normal, next_direction));
            throughput *= bsdf * cosine / pdf;
            std::cout << "throughput" << throughput << std::endl;
            next_ray = Ray(info.position + 0.01f * next_direction, next_direction);

        }
        if (LTE[0] < 1.0)std::cout << "LTE" << LTE << std::endl;
        return LTE;
    }
    std::string getIntegratorType()const {
        return "PathTrace";
    }
};