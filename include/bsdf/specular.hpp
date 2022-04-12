#pragma once 
#include "bsdf/bsdf.hpp"
#include "math/vec3.h"
#include <cmath>
#include <memory>

class Specular :public BSDF {
private:
    Vec3 rho;

public:
    Specular(const Vec3& rho) :rho(rho) {};

    Vec3 samplingBSDF(const Vec3& wo, Vec3& wi, float& pdf,
        std::shared_ptr<Sampler>& sampler) const override {
        pdf = 1.0;
        wi = reflect(wo, Vec3(0, 1, 0));

        return rho / std::abs(wi[1]);
    }
    Vec3 evaluateBSDF(const Vec3& wo, const Vec3& wi) const override { return Vec3(0.0); }
    float samplePDF(const Vec3& wo, const Vec3& wi)const override {
        return 0;
    }
    std::string getBSDFname()const override { return "Speculer"; }
};