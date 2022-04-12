#pragma once 
#include "math/vec3.h"
#include "math/vec2.h"
#include "image/texture.h"
#include "bsdf.hpp"
#include "lambert.hpp"
#include "ggx.hpp"
#include "specular.hpp"
#include "glass.hpp"
#include <string>
#include <cmath>
#include <memory>


class Material {
public:
    virtual std::shared_ptr<BSDF> getBSDF(const Vec2& uv) const = 0;
    virtual std::string getMaterialName()const = 0;
};

class Diffuse :public Material {
private:
    std::shared_ptr<Texture> _baseColor;
public:
    Diffuse(const Vec3& col) {
        _baseColor = std::make_shared<Texture>(col);
    }
    Diffuse(const std::shared_ptr<Texture>& _baseColor) :_baseColor(_baseColor) {}

    std::shared_ptr<BSDF> getBSDF(const Vec2& uv)const {
        Vec3 rho = _baseColor->getTex(uv[0], uv[1]);
        return std::make_shared<Lambert>(rho);
    }

    std::string getMaterialName()const {
        return "Diffuse";
    }
};

class IdealGlass : public Material {
private:
    std::shared_ptr<Texture> _basecolor;
    std::shared_ptr<Texture> _ior;
public:
    IdealGlass(const Vec3& col, const float& ior) {
        _basecolor = std::make_shared<Texture>(col);
        _ior = std::make_shared<Texture>(ior);
    }
    IdealGlass(const std::shared_ptr<Texture>& _basecolor, const std::shared_ptr<Texture>& _ior) :_basecolor(_basecolor), _ior(_ior) {}

    std::shared_ptr<BSDF> getBSDF(const Vec2& uv)const {
        Vec3 rho = _basecolor->getTex(uv[0], uv[1]);
        float ior = _ior->getTex(uv[0], uv[1])[0];

        return std::make_shared<Glass>(rho, ior);
    }
    std::string getMaterialName()const {
        return "IdealGlass";
    }
};

class Metallic :public Material {
private:
    std::shared_ptr<Texture> _baseColor;
    std::shared_ptr<Texture> _roughness;
    std::shared_ptr<Texture> _anisotropic;
    std::shared_ptr<Texture> _rotatetangent;
public:
    Metallic(const Vec3& col, const float& roughness, const float& anisotoropic = 0.0f, const float& rotatetangent = 0.0f) {
        _baseColor = std::make_shared<Texture>(col);
        _roughness = std::make_shared<Texture>(Vec3(roughness));
        _anisotropic = std::make_shared<Texture>(Vec3(anisotoropic));
        _rotatetangent = std::make_shared<Texture>(Vec3(rotatetangent));
    }
    Metallic(const std::shared_ptr<Texture>& _baseColor, const std::shared_ptr<Texture>& _roughness,
        const std::shared_ptr<Texture>& anisotoropic, const std::shared_ptr<Texture>& rotatetangent) :
        _baseColor(_baseColor), _roughness(_roughness), _anisotropic(anisotoropic), _rotatetangent(rotatetangent) {}


    std::shared_ptr<BSDF> getBSDF(const Vec2& uv)const {
        Vec3 rho = _baseColor->getTex(uv[0], uv[1]);
        float roughness = _roughness->getTex(uv[0], uv[1])[0];
        float anisotropic = _anisotropic->getTex(uv[0], uv[1])[0];
        float rotateTangent = _rotatetangent->getTex(uv[0], uv[1])[0];
        return std::make_shared<GGX_VisibleNormal>(rho, roughness, anisotropic, rotateTangent);
    }
    std::string getMaterialName()const {
        return "Metallic";
    }
};

class Mirror : public Material {
private:
    std::shared_ptr<Texture> _baseColor;
public:
    Mirror(const Vec3& col) {
        _baseColor = std::make_shared<Texture>(col);
    }
    Mirror(const std::shared_ptr<Texture>& _baseColor) :_baseColor(_baseColor) {}

    std::shared_ptr<BSDF> getBSDF(const Vec2& uv)const {
        Vec3 rho = _baseColor->getTex(uv[0], uv[1]);
        return std::make_shared<Specular>(rho);
    }
    std::string getMaterialName()const {
        return "Mirror";
    }
};