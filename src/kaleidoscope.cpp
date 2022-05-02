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
    const unsigned int width = 512, height = 512;

    auto worldtex = std::make_shared<WorldTexture>("../texture/TestHDR2.hdr");

    auto lit1 = std::make_shared<Light>(Vec3(1.0) * 3.0);

    auto mat1 = std::make_shared<Mirror>(Vec3(0.9));
    auto mat2 = std::make_shared<Diffuse>(Vec3(0.9));
    auto mat3 = std::make_shared<Diffuse>(Vec3(0.2, 0.8, 0.2));
    Vec3 cameraPos(0, 10, 0);
    Vec3 cameraDir = normalize(Vec3(0, 0.0, 0) - cameraPos);


    auto camera = std::make_shared<PinholeCamera>(cameraPos, cameraDir, 2.0f);

    auto integrator = std::make_shared<PathTracer>();
    auto integrator1 = std::make_shared<UVChecker>();

    auto sampler = std::make_shared<RNGrandom>();
    Scene scene;

    scene.setSkySphere(Vec3(2.0));
    scene.addPolygon("../model/kaleidoscope.obj", mat1);
    scene.addPolygon("../model/torus.obj", mat3);
    scene.addPolygon("../model/boxs.obj", mat2);
    scene.SceneBuild();

    Renderer renderer;
    renderer.rendererSet(width, height, integrator, camera, 100);
    renderer.Render(scene, "kaleidoscope_test", sampler);

    // renderer.TimeLimitRender(scene, "MIS-Sample1", sampler, 60000);
    // renderer.rendererSet(width, height, integrator2, camera, 100);
    // renderer.TimeLimitRender(scene, "IBL-PTtest", sampler, 60000);
    // renderer.rendererSet(width, height, integrator1, camera, 100);
    // renderer.TimeLimitRender(scene, "IBL-NEEtest", sampler, 60000);
    return 0;
}