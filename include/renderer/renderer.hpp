#pragma once
#include <omp.h>
#include <iostream>
#include <chrono>   
#include <memory>
#include <string>
#include "renderer/renderer.hpp"
#include "image/image.h"
#include "scene/scene.h"
#include "core/ray.h"
#include "sampling/sampler.hpp"
#include "sampling/rng.hpp"
#include "integrator/integrator.hpp"
#include "camera/camera.hpp"

class Renderer {
private:
    unsigned int width, height;
    std::shared_ptr<Integrator> integrator;
    std::shared_ptr<Camera> camera;

    unsigned int sampling;

public:
    Renderer() {}

    void rendererSet(unsigned int w, unsigned int h, const std::shared_ptr<Integrator>& inte, const std::shared_ptr<Camera>& cam, unsigned int sample) {
        width = w;
        height = h;
        integrator = inte;
        camera = cam;
        sampling = sample;
    }
    void changeIntegrator(std::shared_ptr<Integrator>& inte) {
        integrator = inte;
    }
    void changeSampling(unsigned int sample) {
        sampling = sample;
    }

    void Render(const Scene& scene, const std::string& filename, const std::shared_ptr<Sampler>& insampler) {
        auto image = std::make_shared<Image>(width, height);
        std::cout << std::endl << "----------------------" << std::endl;
        std::cout << "Rendering start" << std::endl;
        std::cout << "----------------------" << std::endl;

        std::cout << std::endl << "Integrator Type : " << integrator->getIntegratorType() << std::endl;
        std::cout << "Camera Type : " << camera->getCameraType() << std::endl;
        std::cout << "Image Width : " << width << " , Image Height : " << height << std::endl;
        std::cout << "Sample : " << sampling << std::endl;

        auto start = std::chrono::system_clock::now();
#pragma omp parallel for schedule(dynamic,1)
        for (int j = 0; j < height; j++) {
            for (int i = 0; i < width; i++) {
                Vec3 sumRadiance;
                std::shared_ptr<Sampler> sampler = std::make_shared<RNGrandom>();
                sampler->setSeed(i + j * width);
                for (int k = 0; k < sampling; k++) {
                    // std::cout << k << std::endl;
                    // std::cout << sampler->getSample() << std::endl;
                    // std::cout << sampler->getSample() << std::endl;
                    // std::cout << sampler->getSample() << std::endl;
                    // std::cout << sampler->getSample() << std::endl;
                    float u = (2.0f * (i + sampler->getSample() - 0.5f) - width) / height;
                    float v = (2.0f * (j + sampler->getSample() - 0.5f) - height) / height;

                    Ray cameraRay = camera->getCameraRay(u, v);
                    Vec3 radiance = integrator->integrate(cameraRay, scene, sampler);
                    // if (radiance[0] < 1.0)std::cout << "radiance" << radiance << std::endl;
                    sumRadiance += radiance;
                }

                image->setPixel(i, j, sumRadiance / static_cast<float>(sampling));
            }
        }

        auto end = std::chrono::system_clock::now();
        auto time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        image->writePNG(filename);

        std::cout << std::endl << "Rendering finish" << std::endl;
        std::cout << "Rendering Time is " << time << "ms" << std::endl;
        std::cout << std::endl << "----------------------" << std::endl;
        std::cout << "Rendering End" << std::endl;
        std::cout << "----------------------" << std::endl;
    }


};