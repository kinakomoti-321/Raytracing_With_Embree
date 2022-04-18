#pragma once 
#include "../math/vec3.h"
#include "../core/ray.h"
#include "camera/camera.hpp"
#include <string>
class PinholeCamera :public Camera {
private:
    Vec3 origin;
    Vec3 cameraDir;
    Vec3 cameraUp;
    Vec3 cameraSide;
    float p;
public:
    PinholeCamera(const Vec3& origin, const Vec3& cameraDir, const float p) : origin(origin), cameraDir(cameraDir), p(p) {
        Vec3 t, b;
        tangentSpaceBasis(cameraDir, t, b);
        cameraUp = t;
        cameraSide = b;
    }

    Ray getCameraRay(float u, float v, const std::shared_ptr<Sampler>& sampler, float& weight) const override {
        Ray ray;
        ray.origin = origin;
        ray.direction = normalize(cameraDir * p - cameraSide * v + cameraUp * u);
        weight = 1.0;
        return ray;
    }

    std::string getCameraType()const {
        return "PinholeCamera";
    }
};