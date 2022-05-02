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
        lightScale = scale;
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
        float theta = std::acos(std::clamp(dir[1], -1.0f, 1.0f));
        // float phi = std::acos(dir[0] / sqrt(dir[0] * dir[0] + dir[2] * dir[2])) * ((dir[2] < 0.0) ? 1.0f : -1.0f);
        float p = std::atan2(dir[2], dir[0]);
        float phi = (p < 0) ? (p + PI2) : p;

        uv[1] = std::clamp(theta * PI_INV, 0.0f, 1.0f);
        uv[0] = std::clamp(phi * PI_INV2, 0.0f, 1.0f);
        // std::cout << dir << std::endl;
        // std::cout << uv << std::endl;
        return uv;

    }
    Vec3 Le(const Vec3& rayDir)const {
        Vec2 uv = calcUVSphere(rayDir);
        return LE->getTex(uv[0], uv[1]) * lightScale;
    }

    Vec3 enviromentSmapling(const Vec2& sample, float& pdf, Vec3& le) const {
        Vec2 uv = LE->getUVsample(sample, pdf);
        le = LE->getTex(uv[0], uv[1]);

        float theta = uv[0] * PI, phi = uv[1] * PI2;
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
            //LightRandomSampling
            // Vec3 lightsampleDir = SphereSampling(sampler->getSample(), sampler->getSample(), pdf);
            // lightDirection = localToWorld(lightsampleDir, t, normal, b);
            // LightLe = Le(lightDirection);
            // is_directionalSample = false;
            // pdf *= (hasDireLight) ? 0.5f : 1.0f;


            //LightImportanceSampling
            lightDirection = enviromentSmapling(Vec2(sampler->getSample(), sampler->getSample()), pdf, LightLe);
            is_directionalSample = false;
            LightLe *= lightScale;
        }

        info.distance = 1000.0;
        info.normal = -lightDirection;
        // LightLe = Vec3(1.0);

        return lightDirection;
    }

    float skyLightPointPDF(const Vec3& dir)const {
        Vec2 uv = this->calcUVSphere(dir);
        float pdf = LE->getUVpdf(uv);
        pdf /= (2.0f * PI * PI * std::sin(uv[0] * PI));
        return pdf * ((hasDirectionalLight) ? 0.5f : 1.0f);

    }
};