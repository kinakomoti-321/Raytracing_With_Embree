#include "renderer/renderer.hpp"
#include "camera/camera.hpp"
#include "camera/pinholecamera.h"
#include "bsdf/bsdf.hpp"
#include "bsdf/lambert.hpp"
#include "integrator/integrator.hpp"
#include "integrator/normalchecker.hpp"
#include "integrator/pathtracer_lighttex.hpp"
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

    auto littex = std::make_shared<Texture>("../texture/scene4/cube_emission.png");

    auto mat2 = std::make_shared<Metallic>(Vec3(0.9), 0.3, 0.0, 0.0);
    auto mat3 = std::make_shared<Metallic>(Vec3(0.9), 0.2, 0.0, 0.0);

    auto lit1 = std::make_shared<Light>(littex, 30.0f);
    auto lit2 = std::make_shared<Light>(Vec3(0.52, 0.77, 1.0) * 10.0f);

    Vec3 cameraPos(6.0, 4.0, 6.0);
    Vec3 cameraDir = normalize(Vec3(0, 5.0, 0.0) - cameraPos);

    auto camera = std::make_shared<PinholeCamera>(cameraPos, cameraDir, 2.0f);
    auto camera1 = std::make_shared<ThinLens>(cameraPos, cameraDir, 1, PI / 4.0, 55.0);
    camera1->forcus(Vec3(-0.5, 1, 0));
    auto integrator = std::make_shared<PathTracer_LightTextureON>();
    auto sampler = std::make_shared<RNGrandom>();
    Scene scene;
    scene.setSkySphere(Vec3(0.0001));

    scene.addPolygon("../model/scene4/scene4_stage.obj", mat2);
    scene.addPolygon("../model/scene4/scene4_lightBox.obj", mat3, lit1);
    // scene.addPolygon("../model/scene2/scene4_lightBox.obj", mat3);
    scene.SceneBuild();

    Renderer renderer;
    renderer.rendererSet(width, height, integrator, camera, 1000);
    renderer.Render(scene, "scene4", sampler);

    return 0;
}