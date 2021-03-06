#pragma once
#include "integrator.hpp"

class NEE : public Integrator {
    Vec3 integrate(const Ray& ray, const Scene& scene, std::shared_ptr<Sampler>& sampler) const {
        const int MaxDepth = 100;
        float p = 0.99;
        Vec3 throughput(1.0);
        Vec3 LTE(0);
        Ray next_ray = ray;
        // std::cout << "start" << std::endl;
        for (int depth = 0; depth < MaxDepth; depth++) {
            // Russian roulette
            p = std::min(std::max(std::max(throughput[0], throughput[1]), throughput[2]), 1.0f);
            if (sampler->getSample() > p) break;
            throughput /= p;

            IntersectInfo info;
            if (!scene.Intersection(next_ray, info)) {
                if (depth == 0) {
                    LTE = throughput * scene.getSkyLe(next_ray.direction);
                }
                break;
            }

            if (scene.faceHasLight(info.FaceID)) {
                if (depth == 0) {
                    LTE += throughput * scene.faceLight(info.FaceID)->le();
                }
                break;
            }


            // wo: 入射方向,wi:反射方向
            Vec3 wo = -next_ray.direction;
            Vec3 wi;
            float pdf;
            Vec3 bsdf;

            Vec3 t, b;
            tangentSpaceBasis(info.normal, t, b);

            auto mat = scene.faceMaterial(info.FaceID);
            auto material = mat->getBSDF(info.texcoord);
            // std::cout << "Test!" << std::endl;
            //光源サンプリング
            {
                IntersectInfo lightInfo;
                lightInfo.position = info.position;
                lightInfo.normal = info.normal;
                Vec3 lightLe;
                bool is_sceneSample;
                bool is_directionalSample;
                Vec3 lightDir =
                    scene.lightPointSampling(pdf, sampler, lightInfo, lightLe, is_sceneSample, is_directionalSample);

                Ray shadowRay(info.position, lightDir);

                // std::cout << std::endl;
                // std::cout << lightDir << std::endl;
                // std::cout << lightLe << std::endl;
                // std::cout << pdf << std::endl;
                // std::cout << lightInfo.distance << std::endl;

                shadowRay.maxt = lightInfo.distance - 0.001f;
                IntersectInfo shadowInfo;

                if (!scene.Intersection(shadowRay, shadowInfo)) {
                    float cosine1 = std::abs(dot(info.normal, lightDir));
                    float cosine2 = std::abs(dot(lightInfo.normal, -lightDir));

                    wi = lightDir;

                    Vec3 local_wo = worldtoLocal(wo, t, info.normal, b);
                    Vec3 local_wi = worldtoLocal(wi, t, info.normal, b);

                    bsdf = material->evaluateBSDF(local_wo, local_wi);

                    float G = (is_sceneSample) ? 1.0f : cosine2 / (lightInfo.distance * lightInfo.distance);


                    // std::cout << "cosine1" << cosine1 << std::endl;
                    // std::cout << cosine2 << std::endl;
                    // std::cout << wi << std::endl;
                    // std::cout << bsdf << std::endl;
                    // std::cout << G << std::endl;;
                    // std::cout << throughput * (bsdf * G * cosine1 / pdf) * lightLe << std::endl;
                    // std::cout << std::endl;
                    LTE += throughput * (bsdf * G * cosine1 / pdf) * lightLe;
                }
            }
            wo = worldtoLocal(-next_ray.direction, t, info.normal, b);
            // BSDF計算
            bsdf = material->samplingBSDF(wo, wi, pdf, sampler);

            const Vec3 next_direction = localToWorld(wi, t, info.normal, b);
            const float cosine = std::abs(dot(info.normal, next_direction));
            throughput *= bsdf * cosine / pdf;

            // DebugLog("throughput", throughput);

            next_ray = Ray(info.position + 0.001f * next_direction, next_direction);
        }

        return LTE;

    }
    std::string getIntegratorType() const {
        return "Next-Event-Estimation";
    }
};