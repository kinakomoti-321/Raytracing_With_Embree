#pragma once 

#include "integrator/integrator.hpp"
#include <iostream>
class MIS : public Integrator {
public:
    Vec3 integrate(const Ray& ray, const Scene& scene, std::shared_ptr<Sampler>& sample) const {
        float p = 1.0; //Probability Rossian Roulette
        const unsigned int MaxDepth = 100; //Max of RayDepth
        Vec3 LTE = Vec3(0.0); //Result of Radiance
        Vec3 throughput = Vec3(1.0); //ThroughPut of Radiance. Midway Waight
        IntersectInfo info; //information of intersected surface. use as information of Current location
        Ray next_ray = ray; //the ray to shot next

        for (int depth = 0; depth < MaxDepth; depth++) {
            // Russian roulette
            p = std::min(std::max(std::max(throughput[0], throughput[1]), throughput[2]), 1.0f);
            if (sample->getSample() > p) break;
            throughput /= p;


            //Ray shot
            IntersectInfo info;
            if (!scene.Intersection(next_ray, info)) {
                if (depth == 0) {
                    LTE = throughput * scene.getSkyLe(next_ray.direction);
                }
                break;
            }


            if (scene.faceHasLight(info.FaceID)) {
                //Ray into light
                if (depth == 0) {
                    //first shot
                    LTE += throughput * scene.faceLight(info.FaceID)->le();
                }
                break;
            }

            //directions
            // wo: 入射方向,wi:反射方向
            Vec3 t, b;
            tangentSpaceBasis(info.normal, t, b);
            Vec3 wo = localToWorld(-next_ray.direction, t, info.normal, b);
            Vec3 wi;
            float pdf;
            Vec3 bsdf;
            auto mat = scene.faceMaterial(info.FaceID);
            auto material = mat->getBSDF(info.texcoord);

            //NEE
            {
                wo = -next_ray.direction;
                //光源サンプリング
                IntersectInfo lightInfo;
                scene.lightPointSampling(pdf, sample, lightInfo);
                Vec3 lightPos = lightInfo.position;
                Vec3 lightDir = normalize(lightPos - info.position);
                Ray shadowRay(info.position, lightDir);
                lightInfo.distance = norm(lightPos - info.position);

                shadowRay.maxt = lightInfo.distance - 0.001f;
                IntersectInfo shadowInfo;

                if (!scene.Intersection(shadowRay, shadowInfo)) {
                    float cosine1 = std::abs(dot(info.normal, lightDir));
                    float cosine2 = std::abs(dot(lightInfo.normal, -lightDir));

                    wi = lightDir;

                    Vec3 local_wo = worldtoLocal(wo, t, info.normal, b);
                    Vec3 local_wi = worldtoLocal(wi, t, info.normal, b);

                    bsdf = material->evaluateBSDF(local_wo, local_wi);
                    //MISWightの計算
                    float lightPDF = pdf;
                    //幾何項（ヤコビアン）の計算
                    float G = cosine2 / (lightInfo.distance * lightInfo.distance);
                    //PTでのPDF
                    float pathPDF = material->samplePDF(local_wo, local_wi) * G;
                    float MISweight = lightPDF / (lightPDF + pathPDF);
                    //寄与の追加
                    LTE += throughput * MISweight * (bsdf * G * cosine1 / pdf) * scene.faceLight(lightInfo.FaceID)->le();
                }
            }

            //Pathtrace
            {
                //方向サンプリング
                float pathPdf;
                wo = worldtoLocal(-next_ray.direction, t, info.normal, b);
                Vec3 bsdf = material->samplingBSDF(wo, wi, pathPdf, sample);
                const Vec3 nextDir = localToWorld(wi, t, info.normal, b);

                Ray lightRay(info.position, nextDir);
                IntersectInfo lightInfo;

                if (scene.Intersection(lightRay, lightInfo)) {
                    if (scene.faceHasLight(lightInfo.FaceID)) {
                        //衝突かつその物体がLight
                        float cosine1 = absdot(info.normal, nextDir);
                        float cosine2 = absdot(lightInfo.normal, -nextDir);
                        float dis2 = lightInfo.distance * lightInfo.distance;

                        //幾何項(ヤコビアン)の逆数
                        float invG = dis2 / cosine2;

                        //MISweight
                        //NEEでのPDF
                        float lightPdf = scene.lightPointPDF(lightInfo.FaceID, lightInfo.position) * invG;
                        float MISweight = pathPdf / (pathPdf + lightPdf);
                        // MISweight = 1.0f;
                        //Result
                        LTE += throughput * MISweight * cosine1 * scene.faceLight(lightInfo.FaceID)->le() * bsdf / pathPdf;
                    }
                }
            }

            //次に進む方向のサンプリング

            wo = worldtoLocal(-next_ray.direction, t, info.normal, b);
            bsdf = material->samplingBSDF(wo, wi, pdf, sample);

            const Vec3 next_direction = localToWorld(wi, t, info.normal, b);
            const float cosine = std::abs(dot(info.normal, next_direction));
            throughput *= bsdf * cosine / pdf;

            //次のレイ
            next_ray = Ray(info.position + 0.001f * next_direction, next_direction);
        }

        return LTE;
    }

    std::string getIntegratorType()const {
        return "Multiple-Importance-Sampling";
    }

};