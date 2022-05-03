#pragma once

#include "sampling/sampler.hpp"
#include "core/constant.hpp"
#include "math/vec3.h"
#include <memory>
#include <cmath>

class Volume {
public:
    virtual float sampleS(std::shared_ptr<Sampler>& Sampler) const = 0;
    virtual Vec3 phaseFanc(const Vec3& wo, std::shared_ptr<Sampler>& Sampler)const = 0;
    virtual float evaluatePhaseFunc(const Vec3& wo, const Vec3& wi) const = 0;
    virtual float getmueA()const = 0;
    virtual float getmueS()const = 0;
    virtual float getmueT()const = 0;
    virtual float getTransmittance(float t) const = 0;
    virtual Vec3 Le() const = 0;
};

class HomoVolume : public Volume {

private:
    float mueA;
    float mueS;
    float mueT;
    float phaseG;
    Vec3 le;

public:
    HomoVolume(float mueA, float mueS, float phaseG, const Vec3& le) :mueA(mueA), mueS(mueS), phaseG(phaseG), le(le) {
        mueT = mueA + mueS;
    }
    float sampleS(std::shared_ptr<Sampler>& sampler) const override {
        float random = sampler->getSample();
        return -std::log(std::max(1.0f - random, 0.0f)) / mueT;
    };
    Vec3 phaseFanc(const Vec3& wo, std::shared_ptr<Sampler>& Sampler)const override {
        Vec3 wi;
        //woの接空間
        Vec3 b, t;
        tangentSpaceBasis(wo, t, b);

        float u = 2.0f * Sampler->getSample() - 1.0f;
        float v = Sampler->getSample();

        float g = phaseG;
        float cosTheta = 0.0f;
        if (g != 0.0f) {
            float sqrTerm = (1.0f - g * g) / (1.0f + g * u);
            cosTheta = (1.0f + g * g - sqrTerm * sqrTerm) / (2.0f * g);
        }
        else {
            cosTheta = u;
        }

        float sinTheta = std::sqrt(std::max(1.0f - cosTheta * cosTheta, 0.0f));
        float phi = 2.0f * M_PI * v;
        wi = Vec3(sinTheta * std::cos(phi), cosTheta, sinTheta * std::sin(phi));
        wi = localToWorld(wi, t, wo, b);

        return wi;
    };

    float evaluatePhaseFunc(const Vec3& wo, const Vec3& wi)const override {
        float cosine = dot(wo, wi);
        float g2 = phaseG * phaseG;

        return PI_INV4 * (1 - g2) / std::pow(1.0f + g2 + 2 * phaseG * cosine, 1.5f);
    }
    float getmueA()const override {
        return mueA;
    };
    float getmueS()const override {
        return mueS;
    };
    float getmueT()const override {
        return mueT;
    };
    float getTransmittance(float t)const override {
        return std::exp(-mueT * t);
    };
    Vec3 Le() const override {
        return le;
    };
};