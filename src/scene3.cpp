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
#include "bsdf/glass.hpp"
#include "bsdf/ggx.hpp"
#include "integrator/nee.hpp"
#include "integrator/mis.hpp"
#include "integrator/uvchecker.hpp"
#include "integrator/texturechecker.hpp"
#include "volume/volume.hpp"
#include "integrator/volume_homo_renderer.hpp"
#include "bsdf/material.hpp"
#include "camera/thinlens.h"
#include <iostream>
#include <memory>

int main() {
    const unsigned int width = 512 * 2.0, height = 512 * 1.4;

    auto mat1 = std::make_shared<Diffuse>(Vec3(0.9));

    auto lit1 = std::make_shared<Light>(Vec3(1.0, 0.8, 0.6) * 3.0);
    auto lit3 = std::make_shared<Light>(Vec3(1.0, 0.8, 0.6) * 2.0);
    auto lit2 = std::make_shared<Light>(Vec3(1.0, 0.8, 0.6) * 2.0);

    Vec3 cameraPos(3.0, 0.4, -1.5);
    Vec3 cameraDir = normalize(Vec3(0, 1.0, 0.0) - cameraPos);

    auto camera = std::make_shared<PinholeCamera>(cameraPos, cameraDir, 2.0f);
    auto camera1 = std::make_shared<ThinLens>(cameraPos, cameraDir, 1, PI / 4.0, 55.0);
    camera1->forcus(Vec3(-0.5, 1, 0));
    auto integrator = std::make_shared<Volume_homo_render>();
    auto sampler = std::make_shared<RNGrandom>();
    Scene scene;
    scene.setSkySphere(Vec3(0.28, 0.426, 0.439));

    auto vol = std::make_shared<HomoVolume>(5.0, 5.0, -0.8, Vec3(0));
    scene.addPolygon("../model/scene3/scene3_background.obj", mat1);
    scene.addPolygon("../model/scene3/scene3_light1.obj", mat1, lit1);
    scene.addPolygon("../model/scene3/scene3_light2.obj", mat1, lit2);
    scene.addPolygon("../model/scene3/scene3_light3.obj", mat1, lit3);
    scene.addPolygon("../model/scene3/scene3_object.obj", mat1, nullptr, vol);
    scene.SceneBuild();

    Renderer renderer;
    renderer.rendererSet(width, height, integrator, camera, 1000);
    renderer.Render(scene, "scene3_1", sampler);

    return 0;
}