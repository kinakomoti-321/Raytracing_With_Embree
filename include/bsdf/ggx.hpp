#pragma once
#include "bsdf/bsdf.hpp"
#include "core/constant.hpp"
#include <iostream>
#include <cmath>
#include "bsdf/lambert.hpp"
#include "math/vec3.h"
#include <memory>

class GGX :public BSDF {

private:
    Vec3 F0;
    float alpha;
    std::shared_ptr<Texture> tex1;
    std::shared_ptr<Texture> tex2;

    Vec3 Fresnel(const float im) const {
        float delta = std::max(1.0f - im, 0.0f);
        return F0 + (Vec3(1.0f) - F0) * std::pow(delta, 5);
    }
public:
    GGX(const Vec3& F0, const float roughness) :F0(F0) {
        alpha = std::clamp(roughness * roughness, 0.01f, 1.0f);
        tex1 = std::make_shared<Texture>(F0);
        tex2 = std::make_shared<Texture>(Vec3(roughness));
    }
    void textureUVSet(const Vec2& uv) {
        float delta = tex2->getTex(uv[0], uv[1])[0];
        alpha = std::clamp(delta * delta, 0.01f, 1.0f);
        F0 = tex1->getTex(uv[0], uv[1]);
    }

    float G(const Vec3& v, const Vec3& m)const {
        float delta = alpha * BSDFMath::tanTheta(v);
        // float sign = ((dot(v, m) / BSDFMath::cosTheta(v)) > 0.0f) ? 1.0f : 0.0f;
        float sign = 1.0f;
        float result = sign * 2.0f / (1.0f + std::sqrt(1.0f + delta * delta));

        return result;
    }

    float lambda(const Vec3& v) const {
        float delta = std::max(alpha * BSDFMath::tanTheta(v), 0.0f);
        return std::max((-1.0f + std::sqrt(1.0f + delta * delta)) / 2.0f, 0.0f);
    }
    float G2(const Vec3& o, const Vec3& i) const {
        return 1.0f / (1.0f + this->lambda(o) + this->lambda(i));
    }


    Vec3 samplem(float u, float v) const {
        float theta = std::atan(alpha * std::sqrt(v) / std::sqrt(std::max(1.0f - v, 0.0f)));
        float phi = 2.0f * PI * u;
        return Vec3(std::cos(phi) * std::sin(theta), std::cos(theta),
            std::sin(phi) * std::sin(theta));
    }
    float D(const Vec3& m) const {
        const float tan2theta = BSDFMath::tan2Theta(m);
        if (std::isinf(tan2theta)) return 0;
        const float cos4theta = BSDFMath::cos2Theta(m) * BSDFMath::cos2Theta(m);
        const float term = 1.0f + tan2theta / (alpha * alpha);
        return 1.0f / ((PI * alpha * alpha * cos4theta) * term * term);
    }

    Vec3 samplingBSDF(const Vec3& wo, Vec3& wi, float& pdf,
        std::shared_ptr<Sampler>& sampler) const override {

        Vec3 i = wo;
        Vec3 n = Vec3(0.0, 1.0, 0.0);
        Vec3 m = this->samplem(sampler->getSample(), sampler->getSample());
        Vec3 o = reflect(wo, m);
        wi = o;
        if (wi[1] < 0.0f) return Vec3(0.0);

        float im = absdot(i, m);
        float in = absdot(i, n);
        float on = absdot(o, n);

        Vec3 F = this->Fresnel(im);
        float G_ = this->G2(i, o);
        float D_ = this->D(m);

        Vec3 brdf = F * G_ * D_ / (4.0f * in * on);

        pdf = D_ * BSDFMath::cosTheta(m) / (4.0f * absdot(m, o));

        return brdf;
    }

    Vec3 evaluateBSDF(const Vec3& wo, const Vec3& wi) const  override {
        Vec3 i = wo;
        Vec3 o = wi;
        Vec3 m = normalize(i + o);
        Vec3 n = Vec3(0.0, 1.0, 0.0);

        float im = absdot(i, m);
        float in = absdot(i, n);
        float on = absdot(o, n);

        Vec3 F = this->Fresnel(im);
        float G_ = this->G(i, m) * this->G(o, m);
        float D_ = this->D(m);

        Vec3 brdf = F * G_ * D_ / (4.0f * in * on);

        return brdf;
    }

    float samplePDF(const Vec3& wo, const Vec3& wi)const override {
        Vec3 m = normalize(wo + wi);
        float D_ = this->D(m);
        float pdf = D_ * BSDFMath::cosTheta(m) / (4.0f * absdot(m, wo));
        return pdf;
    }

    std::string getBSDFname()const {
        return "GGX";
    }
    void textureUVSet(const Vec2& uv) {
    }
};

class GGX_anistropic :public BSDF {

private:
    Vec3 F0;
    float a_x;
    float a_y;
    float rotateTangent;

    std::shared_ptr<Texture> F0Tex;
    std::shared_ptr<Texture> roughnessTex;
    std::shared_ptr<Texture> anisotoropicTex;
    std::shared_ptr<Texture> rotateTanTex;

    Vec3 Fresnel(const float im) const {
        float delta = std::max(1.0f - im, 0.0f);
        return F0 + (Vec3(1.0f) - F0) * std::pow(delta, 5);
    }



public:
    GGX_anistropic(const Vec3& F0, const float Roughness, const float Anisotropic, const float rotateTan) :F0(F0) {
        float aspect = std::sqrt(1.0 - Anisotropic * 0.9);
        float r2 = Roughness * Roughness;
        a_x = r2 / aspect;
        a_y = r2 * aspect;
        rotateTangent = rotateTan;
        F0Tex = std::make_shared<Texture>(F0);
        roughnessTex = std::make_shared<Texture>(Vec3(Roughness));
        anisotoropicTex = std::make_shared<Texture>(Vec3(Anisotropic));
        rotateTanTex = std::make_shared<Texture>(rotateTan);
    }

    GGX_anistropic(const std::shared_ptr<Texture>& tex1, const std::shared_ptr<Texture>& tex2, const std::shared_ptr<Texture>& tex3, const std::shared_ptr<Texture>& tex4) {
        F0Tex = tex1;
        roughnessTex = tex2;
        anisotoropicTex = tex3;
        rotateTanTex = tex4;
    }

    void textureUVSet(const Vec2& uv) {
        F0 = F0Tex->getTex(uv[0], uv[1]);
        float aspect = std::sqrt(1.0 - anisotoropicTex->getTex(uv[0], uv[1])[0] * 0.9);
        float Roughness = roughnessTex->getTex(uv[0], uv[1])[0];
        float r2 = Roughness * Roughness;
        a_x = r2 / aspect;
        a_y = r2 * aspect;
        rotateTangent = rotateTanTex->getTex(uv[0], uv[1])[0];
    }

    float D(const Vec3& m) const {
        const float tan2theta = BSDFMath::tan2Theta(m);
        if (std::isinf(tan2theta)) return 0;
        const float cos2phi = BSDFMath::cosPhi2(m);
        const float sin2phi = BSDFMath::sinPhi2(m);
        const float cos4theta = BSDFMath::cos2Theta(m) * BSDFMath::cos2Theta(m);

        float term_alpha = cos2phi / (a_x * a_x) + sin2phi / (a_y * a_y);
        float term = 1.0f + tan2theta * term_alpha;
        return 1.0f / (M_PI * a_x * a_y * cos4theta * term * term);
    }

    float G1(const Vec3& v)const {
        return 1.0f / (1.0f + this->lambda(v));
    }

    float lambda(const Vec3& v) const {
        float alpha = std::sqrt(BSDFMath::cosPhi2(v) * a_x * a_x + BSDFMath::sinPhi2(v) * a_y * a_y);
        float delta = std::max(alpha * BSDFMath::tanTheta(v), 0.0f);
        return std::max((-1.0f + std::sqrt(1.0f + delta * delta)) / 2.0f, 0.0f);
    }

    float G2(const Vec3& o, const Vec3& i) const {
        return 1.0f / (1.0f + this->lambda(o) + this->lambda(i));
    }


    Vec3 samplem(float u, float v) const {
        float tanTerm = std::tan(2.0f * PI * u) * a_y / a_x;
        float phi = std::atan(tanTerm);
        if (u >= 0.75) {
            phi += 2.0f * PI;
        }
        else if (u > 0.25) {
            phi += PI;
        }

        const float cos2phi = std::cos(phi) * std::cos(phi);
        const float sin2phi = std::sin(phi) * std::sin(phi);
        const float term_alpha = cos2phi / (a_x * a_x) + sin2phi / (a_y * a_y);
        const float term = v / ((1.0f - v) * term_alpha);
        const float theta = std::atan(std::sqrt(term));

        return Vec3(std::cos(phi) * std::sin(theta), std::cos(theta),
            std::sin(phi) * std::sin(theta));
    }
    Vec3 samplingBSDF(const Vec3& wo, Vec3& wi, float& pdf,
        std::shared_ptr<Sampler>& sampler) const override {

        Vec3 n = Vec3(0.0, 1.0, 0.0);
        Vec3 b(std::cos(rotateTangent), 0, std::sin(rotateTangent));
        Vec3 t(-std::sin(rotateTangent), 0, std::cos(rotateTangent));
        Vec3 rtan = worldtoLocal(wo, b, n, t);

        Vec3 i = rtan;

        Vec3 m = this->samplem(sampler->getSample(), sampler->getSample());
        Vec3 o = reflect(wo, m);
        wi = localToWorld(o, b, n, t);
        if (wi[1] < 0.0f) return Vec3(0.0);

        float im = absdot(i, m);
        float in = absdot(i, n);
        float on = absdot(o, n);

        Vec3 F = this->Fresnel(im);
        float G_ = this->G2(i, o);
        float D_ = this->D(m);

        Vec3 brdf = F * G_ * D_ / (4.0f * in * on);

        pdf = D_ * BSDFMath::cosTheta(m) / (4.0f * absdot(m, o));

        return brdf;
    }

    Vec3 evaluateBSDF(const Vec3& wo, const Vec3& wi) const  override {
        Vec3 i = wo;
        Vec3 o = wi;
        Vec3 m = normalize(i + o);
        Vec3 n = Vec3(0.0, 1.0, 0.0);

        float im = absdot(i, m);
        float in = absdot(i, n);
        float on = absdot(o, n);

        Vec3 F = this->Fresnel(im);
        float G_ = this->G2(i, o);
        float D_ = this->D(m);

        Vec3 brdf = F * G_ * D_ / (4.0f * in * on);

        return brdf;
    }

    float samplePDF(const Vec3& wo, const Vec3& wi)const override {
        Vec3 m = normalize(wo + wi);
        float D_ = this->D(m);
        float pdf = D_ * BSDFMath::cosTheta(m) / (4.0f * absdot(m, wo));
        return pdf;
    }

    std::string getBSDFname() {
        return "GGX_anisotropic";
    }
    void textureUVSet(const Vec2& uv) {
    }
};

class GGX_VisibleNormal :public BSDF {

private:
    Vec3 F0;
    float a_x;
    float a_y;
    float rotateTangent;

    std::shared_ptr<Texture> F0Tex;
    std::shared_ptr<Texture> roughnessTex;
    std::shared_ptr<Texture> anisotoropicTex;
    std::shared_ptr<Texture> rotateTanTex;
    Vec3 Fresnel(const float im) const {
        float delta = std::max(1.0f - im, 0.0f);
        return F0 + (Vec3(1.0f) - F0) * std::pow(delta, 5);
    }

public:
    GGX_VisibleNormal(const Vec3& F0, const float Roughness, const float Anisotropic, const float rotateTan) :F0(F0) {
        float aspect = std::sqrt(1.0 - Anisotropic * 0.9);
        float r2 = Roughness * Roughness;
        a_x = r2 / aspect;
        a_y = r2 * aspect;
        rotateTangent = rotateTan;
        F0Tex = std::make_shared<Texture>(F0);
        roughnessTex = std::make_shared<Texture>(Vec3(Roughness));
        anisotoropicTex = std::make_shared<Texture>(Vec3(Anisotropic));
        rotateTanTex = std::make_shared<Texture>(rotateTan);
    }

    GGX_VisibleNormal(const std::shared_ptr<Texture>& tex1, const std::shared_ptr<Texture>& tex2, const std::shared_ptr<Texture>& tex3, const std::shared_ptr<Texture>& tex4) {
        F0Tex = tex1;
        roughnessTex = tex2;
        anisotoropicTex = tex3;
        rotateTanTex = tex4;
    }

    void textureUVSet(const Vec2& uv) {
        F0 = F0Tex->getTex(uv[0], uv[1]);
        float aspect = std::sqrt(1.0 - anisotoropicTex->getTex(uv[0], uv[1])[0] * 0.9);
        float Roughness = roughnessTex->getTex(uv[0], uv[1])[0];
        float r2 = Roughness * Roughness;
        a_x = r2 / aspect;
        a_y = r2 * aspect;
        rotateTangent = rotateTanTex->getTex(uv[0], uv[1])[0];
    }

    float D(const Vec3& m) const {
        const float tan2theta = BSDFMath::tan2Theta(m);
        if (std::isinf(tan2theta)) return 0;
        const float cos2phi = BSDFMath::cosPhi2(m);
        const float sin2phi = BSDFMath::sinPhi2(m);
        const float cos4theta = BSDFMath::cos2Theta(m) * BSDFMath::cos2Theta(m);

        float term_alpha = cos2phi / (a_x * a_x) + sin2phi / (a_y * a_y);
        float term = 1.0f + tan2theta * term_alpha;
        return 1.0f / (M_PI * a_x * a_y * cos4theta * term * term);
    }

    float G1(const Vec3& v)const {
        return 1.0f / (1.0f + this->lambda(v));
    }

    float lambda(const Vec3& v) const {
        float alpha = std::sqrt(BSDFMath::cosPhi2(v) * a_x * a_x + BSDFMath::sinPhi2(v) * a_y * a_y);
        float delta = std::max(alpha * BSDFMath::tanTheta(v), 0.0f);
        return std::max((-1.0f + std::sqrt(1.0f + delta * delta)) / 2.0f, 0.0f);
    }

    float G2(const Vec3& o, const Vec3& i) const {
        return 1.0f / (1.0f + this->lambda(o) + this->lambda(i));
    }


    Vec3 samplem(const Vec3& V_, float u, float v) const {
        Vec3 V = normalize(Vec3(a_x * V_[0], V_[1], a_y * V_[2]));

        Vec3 n = Vec3(0, 1, 0);
        if (V[1] > 0.99) n = Vec3(1, 0, 0);
        Vec3 T1 = normalize(cross(V, n));
        Vec3 T2 = normalize(cross(T1, V));

        float r = std::sqrt(u);
        float a = 1.0f / (1.0f + V[1]);
        float phi;
        if (a > v) {
            phi = PI * v / a;
        }
        else {
            phi = PI * (v - a) / (1.0f - a) + PI;
        }

        float P1 = r * std::cos(phi);
        float P2 = r * std::sin(phi);
        if (a < v) P2 *= V[1];

        Vec3 N = P1 * T1 + P2 * T2 + std::sqrt(std::max(1.0f - P1 * P1 - P2 * P2, 0.0f)) * V;

        N = normalize(Vec3(a_x * N[0], N[1], a_y * N[2]));

        return N;
    }

    Vec3 samplingBSDF(const Vec3& wo, Vec3& wi, float& pdf,
        std::shared_ptr<Sampler>& sampler) const override {

        Vec3 i = wo;
        Vec3 n = Vec3(0.0, 1.0, 0.0);
        Vec3 m = this->samplem(i, sampler->getSample(), sampler->getSample());
        Vec3 o = reflect(wo, m);
        wi = o;
        if (wi[1] < 0.0f) return Vec3(0.0);

        float im = absdot(i, m);
        float in = absdot(i, n);
        float on = absdot(o, n);

        Vec3 F = this->Fresnel(im);
        float G_ = this->G2(i, o);
        float D_ = this->D(m);

        Vec3 brdf = F * G_ * D_ / (4.0f * in * on);

        pdf = D_ / (4.0f * absdot(m, o));

        pdf *= this->G1(i) * im / absdot(i, n);
        return brdf;
    }

    Vec3 evaluateBSDF(const Vec3& wo, const Vec3& wi) const  override {
        Vec3 i = wo;
        Vec3 o = wi;
        Vec3 m = normalize(i + o);
        Vec3 n = Vec3(0.0, 1.0, 0.0);

        float im = absdot(i, m);
        float in = absdot(i, n);
        float on = absdot(o, n);

        Vec3 F = this->Fresnel(im);
        float G_ = this->G2(i, o);
        float D_ = this->D(m);

        Vec3 brdf = F * G_ * D_ / (4.0f * in * on);

        return brdf;
    }

    float samplePDF(const Vec3& wo, const Vec3& wi)const override {
        Vec3 m = normalize(wo + wi);
        float D_ = this->D(m);
        float pdf = D_ * BSDFMath::cosTheta(m) / (4.0f * absdot(m, wo));
        return pdf;
    }

    std::string getBSDFname()const {
        return "GGX_VisibleNormal";
    }

    void textureUVSet(const Vec2& uv) {
    }
};