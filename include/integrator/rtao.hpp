#pragma once
#include "integrator.hpp"
#include "math/vec3.h"
#include "sampling/sampler.hpp"
#include "bsdf/lambert.hpp"
#include "core/intersectinfo.h"
#include <cmath>
#include <string>
#include <vector>

class RTAO : public Integrator {
public:
    Vec3 integrate(const Ray& ray, const Scene& scene, std::shared_ptr<Sampler>& sampler) const {
        int sampling = 1000;
        IntersectInfo info;
        if (!scene.Intersection(ray, info)) {
            return Vec3(0);
        }

        int occulusion = 0;
        std::vector<Vec3> wi;
        std::vector<float> ui;
        std::vector<Vec3> diri;
        for (int i = 0; i < sampling; i++) {
            Ray shadow;
            shadow.origin = info.position;
            // info.normal *= -1;
            float u = sampler->getSample(), v = sampler->getSample();
            float pdf;
            Vec3 dir = SphereSampling(u, v, pdf);
            // std::cout << dir << std::endl;
            Vec3 t, b;
            tangentSpaceBasis(info.normal, t, b);

            // diri.push_back(info.normal);
            // diri.push_back(t);
            // diri.push_back(b);
            shadow.direction = localToWorld(dir, t, info.normal, b);
            wi.push_back(shadow.direction);
            shadow.maxt = 1.0f;
            shadow.mint = 0.0001f;
            IntersectInfo shadowInfo;
            if (scene.Intersection(shadow, shadowInfo)) occulusion++;
        }
        if (occulusion == sampling) {
            std::cout << "test" << std::endl;
            std::cout << diri << std::endl;
            std::cout << ui << std::endl;
        }

        return Vec3(1.0) * (1 - occulusion / static_cast<float>(sampling));
    }
    std::string getIntegratorType() const {
        return "RTAO";
    }
};