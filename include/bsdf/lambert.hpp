#pragma once 
#include "bsdf.hpp"
#include "core/constant.hpp"
#include "math/vec3.h"
#include <cmath>
class Lambert : public BSDF {
private:
    Vec3 rho;

    Vec3 HemiSphereSampling(float u, float v, float& pdf) const {
        const float theta = std::acos(std::max(1.0f - u, 0.0f));
        float phi = 2.0f * PI * v;
        pdf = PI_INV2;
        return Vec3(std::cos(phi) * std::sin(theta), std::cos(theta),
            std::sin(phi) * std::sin(theta));
    }

public:
    Lambert(const Vec3& rho) : rho(rho) {};

public:
    Vec3 samplingBSDF(const Vec3& wo, Vec3& wi, float& pdf, Sampler& sampler) const {
        float u = sampler.getSample(), v = sampler.getSample();
        wi = HemiSphereSampling(u, v, pdf);
        return rho * PI_INV;
    };
    Vec3 evaluateBSDF(const Vec3& wo, const Vec3& wi) const {
        return rho * PI_INV;
    };

    float samplePDF(const Vec3& wo, const Vec3& wi) const {
        return PI_INV2;
    };
};