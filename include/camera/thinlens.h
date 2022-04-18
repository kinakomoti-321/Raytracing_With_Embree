#pragma once 
#include "camera.hpp"
#include "core/constant.hpp"
#include "sampling/sampler.hpp" 
#include <memory>

class ThinLens : public Camera {
private:
    float a;
    float b;
    float f;
    float R;
    float F;

    Vec3 origin;
    Vec3 cameraDir;
    Vec3 cameraUp;
    Vec3 cameraSide;

public:
    ThinLens(const Vec3& origin, const Vec3& cameraDir,
        const float& dist, const float& delta, const float& F)

        :origin(origin), cameraDir(cameraDir), F(F) {
        a = delta * dist;
        b = (1.0f - delta) * dist;
        f = a * b / (a + b);
        R = f / (2.0 * F);

        Vec3 t, bid;
        tangentSpaceBasis(cameraDir, t, bid);
        cameraUp = t;
        cameraSide = bid;

    }

    Ray getCameraRay(float u, float v,
        const std::shared_ptr<Sampler>& sampler, float& weight)const {

        Vec3 X0 = origin + u * cameraUp + v * cameraSide;

        Vec3 C = origin + cameraDir * a;
        Vec3 e = normalize(C - X0);

        float r0 = R * sampler->getSample();
        float theta0 = PI2 * sampler->getSample();

        Vec3 S = C + r0 * std::cos(theta0) * cameraUp + r0 * std::sin(theta0) * cameraSide;
        Vec3 P = C + e * b / dot(e, cameraDir);
        float pdf = 2.0 * PI / r0;

        Ray camera;
        camera.origin = S;
        camera.direction = normalize(P - S);

        float cosine = std::abs(dot(camera.direction, cameraDir));
        weight = cosine * cosine * cosine * cosine / (pdf * a * a);

        return camera;
    }

    std::string getCameraType() const {
        return "Thin-Lens";
    }
};