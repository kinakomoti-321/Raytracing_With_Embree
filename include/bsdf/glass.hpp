#pragma once
#include <cmath>
#include <memory>
#include <iostream>
#include "bsdf/bsdf.hpp"
#include "math/vec3.h"
#include "bsdf/specular.hpp"
#include "image/texture.h"

inline bool refract(const Vec3& v, const Vec3& n, float ior1, float ior2,
    Vec3& r) {
    const Vec3 t_h = -ior1 / ior2 * (v - dot(v, n) * n);

    // 全反射
    if (norm2(t_h) > 1.0) return false;

    const Vec3 t_p = -std::sqrt(std::max(1.0f - norm2(t_h), 0.0f)) * n;
    r = t_h + t_p;

    return true;
}

inline float fresnel(const Vec3& w, const Vec3& n, float ior1, float ior2) {
    const float f0 = std::pow((ior1 - ior2) / (ior1 + ior2), 2.0f);
    return f0 + (1.0f - f0) * std::pow(1.0f - std::abs(dot(w, n)), 5.0f);
}

class Glass : public BSDF {
private:
    Vec3 rho;
    float ior;

public:
    Glass(const Vec3& rho, const float& ior) :rho(rho), ior(ior) {};


    Vec3 samplingBSDF(const Vec3& wo, Vec3& wi, float& pdf,
        std::shared_ptr<Sampler>& sampler)const {
        float ior1, ior2;
        Vec3 n;
        if (wo[1] > 0) {
            ior1 = 1.0f;
            ior2 = ior;
            n = Vec3(0, 1, 0);
        }
        else {
            ior1 = ior;
            ior2 = 1.0f;
            n = Vec3(0, -1, 0);
        }

        const float fr = fresnel(wo, n, ior1, ior2);

        if (sampler->getSample() < fr) {
            wi = reflect(wo, n);
            pdf = 1.0f;
            return rho / std::abs(wi[1]);
        }
        else {
            Vec3 t;
            if (refract(wo, n, ior1, ior2, t)) {
                wi = t;
                pdf = 1.0f;
                return rho / std::abs(wi[1]);
                return Vec3(0.0);
            }
            else {
                wi = reflect(wo, n);
                pdf = 1.0f;
                return rho / std::abs(wi[1]);
                return Vec3(0.0);
            }
        }
    };
    Vec3 evaluateBSDF(const Vec3& wo, const Vec3& wi) const override {
        return Vec3(0.0);
    };
    float samplePDF(const Vec3& wo, const Vec3& wi)const override {
        return 0;
    };
    std::string getBSDFname()const {
        return "GLASS";
    }
};