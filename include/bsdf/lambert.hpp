#pragma once 
#include "bsdf.hpp"
#include "core/constant.hpp"
#include "math/vec3.h"
#include <cmath>
#include <algorithm>
#include "image/texture.h"

inline Vec3 cosineSampling(float u, float v, float& pdf) {
    const float theta =
        std::acos(std::clamp(1.0f - 2.0f * u, -1.0f, 1.0f)) / 2.0f;
    const float phi = 2.0f * PI * v;
    pdf = std::cos(theta) / PI;
    return Vec3(std::cos(phi) * std::sin(theta), std::cos(theta),
        std::sin(phi) * std::sin(theta));
}

inline Vec3 SphereSampling(float u, float v, float& pdf) {
    const float theta = std::acos(std::max(1.0f - u, 0.0f));
    // std::cout << theta << std::endl;
    const float phi = 2.0f * PI * v;
    // std::cout << phi << std::endl;
    pdf = 1.0f / (2.0f * PI);
    return Vec3(std::cos(phi) * std::sin(theta), std::cos(theta),
        std::sin(phi) * std::sin(theta));
}

class Lambert : public BSDF {
private:
    Vec3 rho;
public:
    Lambert(const Vec3& rho) : rho(rho) {}

    Vec3 samplingBSDF(const Vec3& wo, Vec3& wi, float& pdf, std::shared_ptr<Sampler>& sampler) const override {
        wi = cosineSampling(sampler->getSample(), sampler->getSample(), pdf);
        // return rho * PI_INV;
        return rho * PI_INV;
    }
    Vec3 evaluateBSDF(const Vec3& wo, const Vec3& wi)const override {
        return rho * PI_INV;
    }
    float samplePDF(const Vec3& wo, const Vec3& wi)const override {
        return std::abs(wi[1]);
    }
    std::string getBSDFname()const {
        return "Lambert";
    }
};