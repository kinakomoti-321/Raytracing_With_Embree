#pragma once 
#include <embree3/rtcore.h>
#include "../math/vec3.h"
#include <limits>
struct Ray{
    Vec3 origin;
    Vec3 direction;
    float mint;
    float maxt; 

    Ray(){
        origin = Vec3(0);
        direction = Vec3(0);
        mint = 0.0;
        maxt = std::numeric_limits<float>::infinity();
    }

    Ray(const Vec3& origin,const Vec3& direction): origin(origin),direction(direction)
    {
       mint = 0.0;
       maxt = std::numeric_limits<float>::infinity(); 
    }

    Ray(const Vec3& origin,const Vec3& direction,const float mint,const float maxt): origin(origin),direction(direction),mint(mint),maxt(maxt){}

    RTCRayHit RayConvertRTCRayHit() const {
        RTCRayHit ray;
        ray.ray.org_x = origin[0];
        ray.ray.org_y = origin[1];
        ray.ray.org_z = origin[2];

        ray.ray.dir_x = direction[0];
        ray.ray.dir_y = direction[1];
        ray.ray.dir_z = direction[2];
    
        ray.ray.tnear = 0.0;
        ray.ray.tfar = std::numeric_limits<float>::infinity();
        ray.hit.geomID = RTC_INVALID_GEOMETRY_ID;

        return ray;
    }

    Vec3 post(float t) const {return origin + t * direction;}
};