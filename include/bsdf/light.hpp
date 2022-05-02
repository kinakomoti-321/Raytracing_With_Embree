#pragma once
#include "math/vec3.h"
#include "image/texture.h"
#include <memory>
class Light {
private:
    std::shared_ptr<Texture> _lightTex;
    float _scale = 1.0f;
public:
    Light(const Vec3& radiance) {
        _lightTex = std::make_shared<Texture>(radiance);
    }
    Light(const std::shared_ptr<Texture>& tex, float scale) {
        _lightTex = tex;
        _scale = scale;
    }

    Vec3 le() const {
        return _lightTex->getTex(0, 0) * _scale;
    }

    Vec3 le(const Vec2& uv) const {
        return _lightTex->getTex(uv[0], uv[1]) * _scale;
    }
};