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

public:
    Sky() {
        LE = std::make_shared<WorldTexture>(Vec3(0.0));
    }

    Sky(const Vec3& Le) {
        LE = std::make_shared<WorldTexture>(Le);
    }

    Sky(const std::shared_ptr<WorldTexture>& le) {
        LE = le;
    }

    Sky(const Vec3& LightDir, const Vec3& LightLe) {
        DirectionalLight = normalize(LightDir);
        DirectionalLightLe = LightLe;
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

        if (norm(DirectionalLight) == 0.0f) {
            Vec3 lightsampleDir = SphereSampling(sampler->getSample(), sampler->getSample(), pdf);
            lightDirection = localToWorld(lightsampleDir, t, normal, b);
            LightLe = Le(lightDirection);
        }
        else {
            lightDirection = normalize(Vec3(1, 1, -1));
            pdf = 1.0f;
            LightLe = DirectionalLightLe;
        }

        info.distance = 1000.0;
        info.normal = -lightDirection;
        // LightLe = Vec3(1.0);

        return lightDirection;
    }
};