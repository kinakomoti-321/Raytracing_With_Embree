#pragma once

#include "sampling/sampler.hpp"
#include "integrator/integrator.hpp"
#include "core/constant.hpp"
class Volume_homo_render :public Integrator {
public:
    Vec3 integrate(const Ray& ray, const Scene& scene,
        std::shared_ptr<Sampler>& sample) const {

        float p = 1.0; //Probability Rossian Roulette
        const unsigned int MaxDepth = 100; //Max of RayDepth
        Vec3 LTE = Vec3(0.0); //Result of Radiance
        Vec3 throughput = Vec3(1.0); //ThroughPut of Radiance. Midway Waight
        IntersectInfo info; //information of Intersectioned surface. use as information of Current location
        Ray next_ray = ray; //the ray to shot next

        bool is_inVolume = false;

        for (int depth = 0; depth < MaxDepth; depth++) {
            // Russian roulette
            p = std::min(std::max(std::max(throughput[0], throughput[1]), throughput[2]), 1.0f);
            if (sample->getSample() > p) break;
            throughput /= p;


            IntersectInfo info;
            //Ray shot
            if (!scene.Intersection(next_ray, info)) {
                LTE = throughput * scene.getSkyLe(next_ray.direction);
                break;
            }

            if (scene.faceHasLight(info.FaceID)) {
                LTE = throughput * scene.faceLight(info.FaceID)->le();
                break;
            }

            if (scene.faceHasVolume(info.FaceID)) {
                // DebugLog("Volume_render");
                Vec3 volDir = next_ray.direction;
                Ray vol_ray = Ray(info.position + 0.001f * volDir, volDir);
                for (int i = 0;;i++) {
                    // if (i > 40) return LTE;
                    // 
                    p = std::min(std::max(std::max(throughput[0], throughput[1]), throughput[2]), 1.0f);
                    if (sample->getSample() > p) break;
                    throughput /= p;

                    IntersectInfo volInfo;
                    scene.Intersection(vol_ray, volInfo);
                    auto vol = scene.faceVolume(info.FaceID);

                    float t = vol->sampleS(sample);
                    //Term2の処理
                    if (t >= volInfo.distance) {
                        next_ray = Ray(volInfo.position + 0.001f * volDir, volDir);
                        break;
                    }

                    //Term1の処理
                    //イベントのロシアンルーレット
                    float mueT = vol->getmueT();
                    float mueA = vol->getmueA();
                    // //吸収イベント
                    if (sample->getSample() < (mueA / mueT)) {
                        // throughput *
                        vol_ray = Ray(vol_ray.origin + t * volDir, volDir);
                        LTE += vol->Le();
                        return LTE;
                    }
                    // //散乱イベント
                    // else {
                    volDir = vol->phaseFanc(volDir, sample);
                    vol_ray = Ray(vol_ray.origin + t * vol_ray.direction, volDir);
                    throughput *= (1.0f - mueA / mueT);
                    // }

                }
            }
            else {
                //raytracing

                // wo: 入射方向,wi:反射方向
                Vec3 t, b;
                tangentSpaceBasis(info.normal, t, b);
                Vec3 wo = worldtoLocal(-next_ray.direction, t, info.normal, b);
                Vec3 wi;
                float pdf;
                Vec3 bsdf;
                auto mat = scene.faceMaterial(info.FaceID);
                auto material = mat->getBSDF(info.texcoord);
                // BSDF計算
                bsdf = material->samplingBSDF(wo, wi, pdf, sample);

                const Vec3 next_direction = localToWorld(wi, t, info.normal, b);
                const float cosine = std::abs(dot(info.normal, next_direction));
                throughput *= bsdf * cosine / pdf;

                next_ray = Ray(info.position, next_direction);
            }
        }
        return LTE;
    }
    std::string getIntegratorType()const {
        return "Homo-Volume";
    }
};