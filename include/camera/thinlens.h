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
        const float filmsize, const float FOV, const float& Fnumber)
        :origin(origin), cameraDir(cameraDir), F(Fnumber) {
        f = 0.5f * filmsize / std::tan(0.5f * FOV);
        R = 2.0f * f / Fnumber;

        a = 10000.0f;
        b = f;
        std::cout << "b" << b << std::endl;
        Vec3 t, bid;
        tangentSpaceBasis(cameraDir, t, bid);
        cameraUp = t;
        cameraSide = bid;
    }

    void forcus(const Vec3 pos) {
        float zf = dot(pos - origin, cameraDir);
        a = zf - b;
        float delta = 0.5f * ((b - a) - std::sqrt(a + b) * std::sqrt(a + b - 4.0f * f));
        std::cout << "delta" << delta << std::endl;
        a += delta;
        b -= delta;

        std::cout << "number a " << a << std::endl;
        std::cout << "number b" << b << std::endl;
        std::cout << "forcus position" << pos << std::endl;
        std::cout << "forcus depth" << zf << std::endl;
    }
    Ray getCameraRay(float u, float v,
        const std::shared_ptr<Sampler>& sampler, float& weight)const {

        Vec3 X0 = origin + -u * cameraUp + v * cameraSide;

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
        weight = 1.0;

        return camera;
    }

    std::string getCameraType() const {
        return "Thin-Lens";
    }
};