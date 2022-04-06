#pragma once
#include <embree3/rtcore.h>
#include <limits>
#include <iostream>
#include "../core/intersectinfo.h"
#include "../core/ray.h"

class Scene{
    private:
    RTCDevice device;
    RTCScene scene;
    RTCGeometry geom;
    public:
    Scene(){
        device = rtcNewDevice(NULL);
        scene = rtcNewScene(device);
        geom = rtcNewGeometry(device,RTC_GEOMETRY_TYPE_TRIANGLE);
    }

    ~Scene(){
        rtcReleaseScene(scene);
        rtcReleaseDevice(device);
    }

    void addPolygon(){
        float* vb = (float*) rtcSetNewGeometryBuffer(geom,
            RTC_BUFFER_TYPE_VERTEX, 0, RTC_FORMAT_FLOAT3, 3*sizeof(float), 3);
        vb[0] = 0.f; vb[1] = 0.f; vb[2] = 0.f; // 1st vertex
        vb[3] = 1.f; vb[4] = 0.f; vb[5] = 0.f; // 2nd vertex
        vb[6] = 0.f; vb[7] = 1.f; vb[8] = 0.f; // 3rd vertex

        unsigned* ib = (unsigned*) rtcSetNewGeometryBuffer(geom,
            RTC_BUFFER_TYPE_INDEX, 0, RTC_FORMAT_UINT3, 3*sizeof(unsigned), 1);
        ib[0] = 0; ib[1] = 1; ib[2] = 2;
    }

    void SceneBuild(){
        rtcCommitGeometry(geom);
        rtcAttachGeometry(scene, geom);
        rtcReleaseGeometry(geom);
        rtcCommitScene(scene);
    }

    IntersectInfo Intersection(const Ray& inray,IntersectInfo& info)const {
        RTCRayHit ray = inray.RayConvertRTCRayHit();
        RTCIntersectContext context;
        rtcInitIntersectContext(&context);

        rtcIntersect1(scene,&context,&ray);

        IntersectInfo info;
        info.hit = ray.hit.geomID != RTC_INVALID_GEOMETRY_ID;        
        info = info;
    }
};