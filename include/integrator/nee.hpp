#pragma once
#include "integrator.hpp"

class NEE : public Integrator {
    Vec3 integrate(const Ray& ray, const Scene& scene, std::shared_ptr<Sampler>& sampler) const {
        const int MaxDepth = 100;
        float p = 0.99;
        Vec3 throughput(1.0);
        Vec3 LTE(0);
        Ray next_ray = ray;
        for (int depth = 0; depth < MaxDepth; depth++) {
            p = std::min(std::max(std::max(throughput[0], throughput[1]), throughput[2]), 1.0f);
            if (sampler->getSample() > p) break;
            throughput /= p;

            IntersectInfo info;
            if (!scene.Intersection(next_ray, info)) {
                if (depth == 0) {
                    LTE = scene.getSkyLe(next_ray.direction);
                }
                break;
            }

            if (scene.faceHasLight(info.FaceID)) {
                if (depth == 0) {
                    LTE = scene.faceLight(info.FaceID)->le();
                }
                break;
            }

            Vec3 wo = -next_ray.direction;
            Vec3 wi;
            float pdf;
            Vec3 bsdf;

            Vec3 t, b;
            auto material = scene.faceMaterial(info.FaceID);
            tangentSpaceBasis(info.normal, t, b);

            IntersectInfo lightInfo;
            float lightPDF;
            scene.lightPointSampling(lightPDF, sampler, lightInfo);
            Vec3 lightPos = lightInfo.position;
            Vec3 lightDir = normalize(lightPos - info.position);
            Ray shadowRay(info.position, lightDir);
            lightInfo.distance = norm(lightDir - info.position);

            shadowRay.maxt = lightInfo.distance - 0.001;
            IntersectInfo shadowInfo;

            if (!scene.Intersection(shadowRay, shadowInfo)) {
                float cosine1 = std::abs(dot(info.normal, lightDir));
                float cosine2 = std::abs(dot(lightInfo.normal, -lightDir));
                wi = lightDir;
                Vec3 local_wo = worldtoLocal(wo, t, info.normal, b);
                Vec3 local_wi = worldtoLocal(wi, t, info.normal, b);

                bsdf = material->evaluateBSDF(wo, wi);

                float G = cosine2 / (lightInfo.distance * lightInfo.distance);

                LTE += throughput * (bsdf * G * cosine1 / lightPDF) * scene.faceLight(lightInfo.FaceID)->le();
            }

            wo = worldtoLocal(-next_ray.direction, t, info.normal, b);
            bsdf = material->samplingBSDF(wo, wi, pdf, sampler);
            const Vec3 next_direction = localToWorld(wi, t, info.normal, b);
            const float cosine = std::abs(dot(info.normal, next_direction));
            throughput *= bsdf * cosine / pdf;

            next_ray = Ray(info.position, next_direction);

        }
        return LTE;
    }
    std::string getIntegratorType() const {
        return "NEE";
    }
};