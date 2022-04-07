#pragma once
#include "math/vec3.h"
#include "sampling/sampler.hpp"
class BSDF {
public:
    virtual Vec3 samplingBSDF(const Vec3& wo, Vec3& wi, float& pdf, Sampler& sampler) const = 0;
    virtual Vec3 evaluateBSDF(const Vec3& wo, const Vec3& wi)const = 0;
    virtual float samplePDF(const Vec3& wo, const Vec3& wi)const = 0;
};