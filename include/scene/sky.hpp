#pragma once 

#include "math/vec3.h"
#include "image/texture.h"
#include <memory>
#include "bsdf/lambert.hpp"
#include "core/intersectinfo.h"
#include <limits>

class Sky {
private:
    std::shared_ptr<WorldTexture> LE;
    Vec3 DirectionalLight = Vec3(0);
    Vec3 DirectionalLightLe = Vec3(0);
    bool hasDirectionalLight;
public:
    Sky() {
        LE = std::make_shared<WorldTexture>(Vec3(0.0));
        hasDirectionalLight = false;
    }

    Sky(const Vec3& Le) {
        LE = std::make_shared<WorldTexture>(Le);
        hasDirectionalLight = false;
    }

    Sky(const std::shared_ptr<WorldTexture>& le) {
        LE = le;
        hasDirectionalLight = false;
    }

    Sky(const Vec3& LightDir, const Vec3& LightLe) {
        DirectionalLight = normalize(LightDir);
        DirectionalLightLe = LightLe;
        LE = std::make_shared<WorldTexture>(Vec3(0.0));
        hasDirectionalLight = true;
    }

    void setDirectionalLight(const Vec3& dir, const Vec3& le) {
        DirectionalLight = normalize(dir);
        DirectionalLightLe = le;
        hasDirectionalLight = true;
    }

    Vec2 calcUVSphere(const Vec3& dir)const {
        Vec2 uv;
        float theta = std::acos(dir[1]);
        float phi = std::acos(dir[0] / sqrt(dir[0] * dir[0] + dir[2] * dir[2])) * ((dir[2] < 0.0) ? 1.0f : -1.0f);

        uv[1] = std::clamp(theta * PI_INV, 0.0f, 1.0f);
        uv[0] = std::clamp((phi + PI) * PI_INV2, 0.0f, 1.0f);
        // std::cout << dir << std::endl;
        // std::cout << uv << std::endl;
        return uv;

    }
    Vec3 Le(const Vec3& rayDir)const {
        Vec2 uv = calcUVSphere(rayDir);
        return LE->getTex(uv[0], uv[1]);
    }

    //Light Direction return
    Vec3 sampleLightSampling(const std::shared_ptr<Sampler>& sampler,
        IntersectInfo& info, float& pdf, Vec3& LightLe)const {

        Vec3 normal = info.normal;
        Vec3 lightDirection;
        Vec3 t, b;

        tangentSpaceBasis(normal, t, b);
        bool hasDireLight = hasDirectionalLight;
        float p = sampler->getSample();
        if (p < 0.5f && hasDireLight) {
            lightDirection = DirectionalLight;
            pdf = 0.5f;
            LightLe = DirectionalLightLe;
        }
        else {
            Vec3 lightsampleDir = SphereSampling(sampler->getSample(), sampler->getSample(), pdf);
            lightDirection = localToWorld(lightsampleDir, t, normal, b);
            LightLe = Le(lightDirection);
            pdf *= (hasDireLight) ? 0.5f : 1.0f;
            // std::cout << pdf << std::endl;
        }

        info.distance = 1000.0;
        info.normal = -lightDirection;
        // LightLe = Vec3(1.0);

        return lightDirection;
    }
    float skyLightPointPDF(const Vec3& dir)const {
        return 1.0f / (2.0f * PI) * ((hasDirectionalLight) ? 0.5f : 1.0f);

    }
};