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
    float lightScale = 1.0;
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

    Sky(const std::shared_ptr<WorldTexture>& le, const float scale) {
        LE = le;
        hasDirectionalLight = false;
        lightScale = 1.0;
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

    Vec3 enviromentSmapling(const Vec2& sample, float& pdf, Vec3& le) const {
        Vec2 uv = LE->getUVsample(sample, pdf);
        le = LE->getTex(uv[0], uv[1]);

        float theta = uv[1] * PI, phi = uv[0] * PI2;
        float costheta = std::cos(theta), sintheta = std::sin(theta);
        float cosphi = std::cos(phi), sinphi = std::sin(phi);

        bool hasDireLight = hasDirectionalLight;
        pdf *= (hasDireLight) ? 0.5f : 1.0f;
        pdf /= (2.0f * PI * PI * sintheta);

        return Vec3(sintheta * cosphi, sintheta * sinphi, costheta);
    }

    //Light Direction return
    Vec3 sampleLightSampling(const std::shared_ptr<Sampler>& sampler,
        IntersectInfo& info, float& pdf, Vec3& LightLe, bool& is_directionalSample)const {

        Vec3 normal = info.normal;
        Vec3 lightDirection;
        Vec3 t, b;

        tangentSpaceBasis(normal, t, b);
        float p = sampler->getSample();
        bool hasDireLight = hasDirectionalLight;
        if (p < 0.5f && hasDireLight) {
            lightDirection = DirectionalLight;
            pdf = 0.5f;
            LightLe = DirectionalLightLe;
            is_directionalSample = true;
        }
        else {
            // Vec3 lightsampleDir = SphereSampling(sampler->getSample(), sampler->getSample(), pdf);
            // lightDirection = localToWorld(lightsampleDir, t, normal, b);
            // LightLe = Le(lightDirection);
            // is_directionalSample = false;
            // pdf *= (hasDireLight) ? 0.5f : 1.0f;

            lightDirection = enviromentSmapling(Vec2(sampler->getSample(), sampler->getSample()), pdf, LightLe);
            is_directionalSample = false;
            LightLe *= lightScale;
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