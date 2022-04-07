#include "renderer/renderer.hpp"
#include "camera/camera.hpp"
#include "camera/pinholecamera.h"
#include "bsdf/bsdf.hpp"
#include "bsdf/lambert.hpp"
#include "integrator/integrator.hpp"
#include "integrator/normalchecker.hpp"
#include "integrator/pathtracer.hpp"
#include "sampling/sampler.hpp"
#include "sampling/rng.hpp"
#include "scene/scene.h"
#include "math/vec3.h"
#include <iostream>
#include <memory>

int main() {
    const unsigned int width = 512, height = 512;

    auto mat1 = std::make_shared<Lambert>(Vec3(1.0));

    Vec3 cameraPos(0, 0, 1);
    Vec3 cameraDir(0, 0, -1);

    auto camera = std::make_shared<PinholeCamera>(cameraPos, cameraDir, 2.0f);

    auto integrator = std::make_shared<PathTracer>();

    auto sampler = std::make_shared<RNGrandom>();
    Scene scene;
    scene.addPolygon("../model/dragon.obj", mat1);
    scene.SceneBuild();

    Renderer renderer;
    renderer.rendererSet(width, height, integrator, camera, 10);
    renderer.Render(scene, "NormalCheck", sampler);
    return 0;
}