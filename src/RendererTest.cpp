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
#include "integrator/testtrace.hpp"
#include "integrator/rtao.hpp"
#include "bsdf/specular.hpp"
#include <iostream>
#include <memory>

int main() {
    const unsigned int width = 512, height = 512;

    auto mat1 = std::make_shared<Lambert>(Vec3(0.9f));
    auto mat2 = std::make_shared<Lambert>(Vec3(0.8, 0.2, 0.2));
    auto mat3 = std::make_shared<Lambert>(Vec3(0.2, 0.8, 0.2));
    auto mat4 = std::make_shared<Lambert>(Vec3(0.2, 0.2, 0.8));
    auto mat5 = std::make_shared<Specular>(Vec3(0.9));

    auto lit1 = std::make_shared<Light>(Vec3(1.0));

    Vec3 cameraPos(0, 0, -3);
    Vec3 cameraDir = normalize(Vec3(0, 0, 0) - cameraPos);


    auto camera = std::make_shared<PinholeCamera>(cameraPos, cameraDir, 2.0f);

    auto integrator = std::make_shared<PathTracer>();

    auto sampler = std::make_shared<RNGrandom>();
    Scene scene;
    scene.addPolygon("../model/dragon.obj", mat5);
    scene.addPolygon("../model/cornel_L.obj", mat2);
    scene.addPolygon("../model/cornel_R.obj", mat3);
    scene.addPolygon("../model/cornelBox.obj", mat1);
    scene.addPolygon("../model/Light.obj", mat1, lit1);
    // scene.addPolygon("../model/CornellBox-Empty-CO.obj", mat1);
    scene.SceneBuild();

    Renderer renderer;
    renderer.rendererSet(width, height, integrator, camera, 1000);
    renderer.Render(scene, "NormalCheck", sampler);
    return 0;
}