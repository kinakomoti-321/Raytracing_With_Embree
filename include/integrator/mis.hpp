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
            // {
            //     // std::cout << "NEE" << std::endl;
            //     wo = -next_ray.direction;
            //     //光源サンプリング
            //     IntersectInfo lightInfo;
            //     lightInfo.position = info.position;
            //     lightInfo.normal = info.normal;
            //     Vec3 lightLe;
            //     bool is_scenesample;
            //     Vec3 lightDir =
            //         scene.lightPointSampling(pdf, sample, lightInfo, lightLe, is_scenesample);
            //     // std::cout << "NEE" << std::endl;
            //     Ray shadowRay(info.position, lightDir);

            //     IntersectInfo shadowInfo;

            //     if (!scene.Intersection(shadowRay, shadowInfo)) {
            //         float cosine1 = std::abs(dot(info.normal, lightDir));
            //         float cosine2 = std::abs(dot(lightInfo.normal, -lightDir));

            //         wi = lightDir;

            //         Vec3 local_wo = worldtoLocal(wo, t, info.normal, b);
            //         Vec3 local_wi = worldtoLocal(wi, t, info.normal, b);

            //         bsdf = material->evaluateBSDF(local_wo, local_wi);
            //         //MISWightの計算
            //         float lightPDF = pdf;
            //         // std::cout << pdf << std::endl;
            //         //幾何項（ヤコビアン）の計算
            //         float G = (!is_scenesample) ? (cosine2 / (lightInfo.distance * lightInfo.distance)) : 1.0f;

            //         //PTでのPDF
            //         float pathPDF = material->samplePDF(local_wo, local_wi) * G;
            //         // std::cout << "NEE" << std::endl;
            //         float MISweight = lightPDF / (lightPDF + pathPDF);

            //         //寄与の追加
            //         // LTE += throughput * MISweight * (bsdf * G * cosine1 / pdf) * lightLe;
            //         LTE += throughput * (bsdf * G * cosine1 / pdf) * lightLe;
            //         // std::cout << "NEE" << std::endl;
            //     }
            //     // std::cout << "Finish" << std::endl;
            // }
            {
                IntersectInfo lightInfo;
                lightInfo.position = info.position;
                lightInfo.normal = info.normal;
                Vec3 lightLe;
                bool is_sceneSample;
                bool is_directionalLight;
                Vec3 lightDir =
                    scene.lightPointSampling(pdf, sample, lightInfo, lightLe, is_sceneSample, is_directionalLight);

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
                    float lightpdf = pdf;
                    float pathpdf = (is_directionalLight) ? 0.0f : material->samplePDF(local_wo, local_wi) * G;
                    float MISweight = lightpdf / (lightpdf + pathpdf);

                    LTE += throughput * MISweight * (bsdf * G * cosine1 / pdf) * lightLe;
                }
            }
            //Pathtrace
            {
                // std::cout << "PathTrace" << std::endl;
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
                        // LTE += throughput * cosine1 * scene.faceLight(lightInfo.FaceID)->le() * bsdf / pathPdf;
                    }
                }
                else {
                    //IBLの処理
                    float lightPDF = scene.skylightPointPDF(nextDir);
                    // std::cout << lightPDF << std::endl;
                    float MISweight = pathPdf / (pathPdf + lightPDF);

                    float cosine1 = absdot(info.normal, nextDir);
                    LTE += throughput * MISweight * cosine1 * scene.getSkyLe(nextDir) * bsdf / pathPdf;
                    // LTE += throughput * cosine1 * scene.getSkyLe(nextDir) * bsdf / pathPdf;
                }
                // std::cout << "Finish" << std::endl;
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