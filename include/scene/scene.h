#pragma once
#include <embree3/rtcore.h>
#include <limits>
#include <iostream>
#include "../core/intersectinfo.h"
#include "../core/ray.h"
#include "polygon.h"
#include "bsdf/bsdf.hpp"
#include <string>
#include <memory>

class Scene {
private:
    RTCDevice device;
    RTCScene scene;
    RTCGeometry geom;

    Polygon poly;
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

    void addPolygon(const std::string& filepath, std::shared_ptr<BSDF> bsdf) {
        poly.AddPolygon(filepath, bsdf);
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

    std::shared_ptr<BSDF> faceMaterial(unsigned int FaceID)const {
        return poly.getMaterial(FaceID);
    }
    std::shared_ptr<Light> faceLight(unsigned int FaceID)const {
        return poly.getLight(FaceID);
    }
    bool faceHasLight(unsigned int FaceID)const {
        return poly.hasLight(FaceID);
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
            info.normal = poly.getFaceNormal(info.FaceID, bary);
            info.normal = poly.getFaceNormal(info.FaceID, bary);
            info.texcoord = poly.getFaceTexcoord(info.FaceID, bary);

            return true;
        }

        else {
            return false;
        }
    }

};