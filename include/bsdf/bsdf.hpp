#pragma once
#include "math/vec3.h"
#include "sampling/sampler.hpp"
#include <string>
#include <memory>
#include <cmath>
#include <algorithm>
class BSDF {
public:
    virtual Vec3 samplingBSDF(const Vec3& wo, Vec3& wi, float& pdf, std::shared_ptr<Sampler>& sampler) const = 0;
    virtual Vec3 evaluateBSDF(const Vec3& wo, const Vec3& wi)const = 0;
    virtual float samplePDF(const Vec3& wo, const Vec3& wi)const = 0;
    virtual std::string getBSDFname()const = 0;
    virtual void textureUVSet(const Vec2& uv) = 0;
};

namespace BSDFMath {
    inline float cosTheta(const Vec3& w) { return w[1]; }
    inline float cos2Theta(const Vec3& w) { return w[1] * w[1]; }
    inline float sinTheta(const Vec3& w) { return std::sqrt(std::max(1.0f - cosTheta(w) * cosTheta(w), 0.0f)); }
    inline float tanTheta(const Vec3& w) { return sinTheta(w) / cosTheta(w); }
    inline float tan2Theta(const Vec3& w) { return tanTheta(w) * tanTheta(w); }

    inline float cosPhi(const Vec3& w) {
        float sintheta = sinTheta(w);
        if (sintheta == 0) return 1.0f;
        return std::clamp(w[0] / sintheta, -1.0f, 1.0f);
    }
    inline float sinPhi(const Vec3& w) {
        float sintheta = sinTheta(w);
        if (sintheta == 0) return 0.0f;
        return std::clamp(w[2] / sintheta, -1.0f, 1.0f);
    }

    inline float cosPhi2(const Vec3& w) { return cosPhi(w) * cosPhi(w); }
    inline float sinPhi2(const Vec3& w) { return sinPhi(w) * sinPhi(w); }

}