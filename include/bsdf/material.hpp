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
};

class Diffuse :public Material {
private:
    std::shared_ptr<Texture> _baseColor;
public:
    Diffuse(const Vec3& col) {
        _baseColor = std::make_shared<Texture>(col);
    }
    Diffuse(std::shared_ptr<Texture> _baseColor) :_baseColor(_baseColor) {}

    std::shared_ptr<BSDF> getBSDF(const Vec2& uv)const {
        Vec3 rho = _baseColor->getTex(uv[0], uv[1]);
        return std::make_shared<Lambert>(rho);
    }
};