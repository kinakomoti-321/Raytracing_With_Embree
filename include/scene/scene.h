#pragma once
#include <embree3/rtcore.h>
#include <limits>
#include <iostream>
#include "../core/intersectinfo.h"
#include "../core/ray.h"
#include "polygon.h"
#include "bsdf/material.hpp"
#include <string>
#include <memory>
#include "sky.hpp"

class Scene {
private:
    RTCDevice device;
    RTCScene scene;
    RTCGeometry geom;

    Polygon poly;
    Sky sky;
public:
    Scene() {
        device = rtcNewDevice(NULL);
        scene = rtcNewScene(device);
        geom = rtcNewGeometry(device, RTC_GEOMETRY_TYPE_TRIANGLE);
    }

    ~Scene() {
        rtcReleaseScene(scene);
        rtcReleaseDevice(device);
    }

    void addPolygon(const std::string& filepath, const std::shared_ptr<Material>& bsdf, const std::shared_ptr<Light>& lit = nullptr, const std::shared_ptr<Volume>& vol = nullptr) {
        poly.AddPolygon(filepath, bsdf, lit, vol);
    }

    void setSkySphere(const Vec3& le) {
        sky = Sky(le);
    }

    void setSkySphere(const std::shared_ptr<WorldTexture>& le, const float scale) {
        sky = Sky(le, scale);
    }

    void setSkyDirectionalLight(const Vec3& dir, const Vec3& le) {
        sky.setDirectionalLight(dir, le);
    }
    void SceneBuild() {
        std::cout << std::endl << "-------------------" << std::endl;
        std::cout << "scene build start" << std::endl;
        std::cout << "-------------------" << std::endl;

        poly.attachGeometry(geom);
        std::cout << "Create Geometry success" << std::endl;

        rtcCommitGeometry(geom);
        std::cout << "Gemetry Commit success" << std::endl;

        rtcAttachGeometry(scene, geom);
        std::cout << "Attach Geometry success" << std::endl;
        rtcReleaseGeometry(geom);
        rtcCommitScene(scene);
        std::cout << "Scene Commit success" << std::endl;
        std::cout << std::endl << "-------------------" << std::endl;
        std::cout << "scene build end" << std::endl;
        std::cout << "-------------------" << std::endl;
    }

    std::shared_ptr<Material> faceMaterial(unsigned int FaceID)const {
        return poly.getMaterial(FaceID);
    }

    std::shared_ptr<Light> faceLight(unsigned int FaceID)const {
        return poly.getLight(FaceID);
    }

    std::shared_ptr<Volume> faceVolume(unsigned int FaceID)const {
        return poly.getVolume(FaceID);
    }

    bool faceHasLight(unsigned int FaceID)const {
        return poly.hasLight(FaceID);
    }

    bool faceHasVolume(unsigned int FaceID)const {
        return faceVolume(FaceID) != nullptr;
    }

    Vec3 getSkyLe(const Vec3& rayDir)const {
        return sky.Le(rayDir);
    }

    //info??????????????????????????????????????????????????????
    Vec3 lightPointSampling(float& pdf, const std::shared_ptr<Sampler>& sampler,
        IntersectInfo& info, Vec3& LightLe, bool& is_sceneSample, bool& is_directionalSample)const {
        float p = sampler->getSample();
        bool hasLight = poly.hasLightScene();
        Vec3 Dir;
        if (p < 0.5 && hasLight) {
            Dir = poly.sampleLightPoint(pdf, sampler, info, LightLe);
            pdf *= 0.5f;
            is_sceneSample = false;
        }
        else {
            is_sceneSample = true;
            Dir = sky.sampleLightSampling(sampler, info, pdf, LightLe, is_directionalSample);
            pdf *= (hasLight) ? 0.5f : 1.0f;
        }
        return Dir;
    }

    float lightPointPDF(unsigned int FaceID, Vec3 lightPos) const {
        return poly.lightPointPDF(FaceID, lightPos) * 0.5f;
    }
    float skylightPointPDF(const Vec3& dir)const {
        return sky.skyLightPointPDF(dir) * ((poly.hasLightScene()) ? 0.5f : 1.0f);
    }

    bool Intersection(const Ray& inray, IntersectInfo& info)const {
        RTCRayHit ray = inray.RayConvertRTCRayHit();
        RTCIntersectContext context;
        rtcInitIntersectContext(&context);

        rtcIntersect1(scene, &context, &ray);


        if (ray.hit.geomID != RTC_INVALID_GEOMETRY_ID) {
            info.distance = ray.ray.tfar;
            info.position = inray.post(info.distance);
            info.FaceID = ray.hit.primID;
            Vec2 bary = Vec2(ray.hit.u, ray.hit.v);
            info.normal = normalize(poly.getFaceNormal(info.FaceID, bary));
            info.texcoord = poly.getFaceTexcoord(info.FaceID, bary);

            return true;
        }

        else {
            return false;
        }
    }

};