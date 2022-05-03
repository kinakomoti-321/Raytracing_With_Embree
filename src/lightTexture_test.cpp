#include "renderer/renderer.hpp"
#include "camera/camera.hpp"
#include "camera/pinholecamera.h"
#include "bsdf/bsdf.hpp"
#include "bsdf/lambert.hpp"
#include "integrator/integrator.hpp"
#include "integrator/pathtracer_lighttex.hpp"
#include "integrator/normalchecker.hpp"
#include "integrator/pathtracer.hpp"
#include "sampling/sampler.hpp"
#include "sampling/rng.hpp"
#include "scene/scene.h"
#include "math/vec3.h"
#include "integrator/testtrace.hpp"
#include "integrator/rtao.hpp"
#include "integrator/debugpathtracer.hpp"
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
    const unsigned int width = 512 * 2, height = 512 * 1.5;

    auto worldtex = std::make_shared<WorldTexture>("../texture/TestHDR2.hdr");

    auto lighttex = std::make_shared<Texture>("../texture/cube_emission.png");
    auto lit1 = std::make_shared<Light>(lighttex, 10.0f);

    auto matDiffuse1 = std::make_shared<Diffuse>(Vec3(0.9));
    auto matDiffuse2 = std::make_shared<Diffuse>(Vec3(0.8, 0.2, 0.2));
    auto matDiffuse3 = std::make_shared<Diffuse>(Vec3(0.2, 0.8, 0.2));
    auto matDiffuse4 = std::make_shared<Diffuse>(Vec3(0.2, 0.2, 0.2));
    auto matMetallic = std::make_shared<Metallic>(Vec3(0.9), 0.1, 0.0, 0.0);
    Vec3 cameraPos = Vec3(1.0, 0.5, -3.0f);
    Vec3 forcus = Vec3(-1, 0, 0);
    Vec3 cameraDir = normalize(forcus - cameraPos);
    auto camera = std::make_shared<PinholeCamera>(cameraPos, cameraDir, 2.0f);

    auto pt = std::make_shared<PathTracer_LightTextureON>();
    auto sampler = std::make_shared<RNGrandom>();
    Scene scene;
    scene.setSkySphere(Vec3(0.001));

    scene.addPolygon("../model/cornelBox.obj", matDiffuse1);
    scene.addPolygon("../model/cornel_L.obj", matDiffuse2);
    scene.addPolygon("../model/cornel_R.obj", matMetallic);
    scene.addPolygon("../model/LightBox.obj", matDiffuse4, lit1);
    // scene.addPolygon("../model/floor.obj", matMetallic);
    // scene.addPolygon("../model/LightPlane.obj", matDiffuse4, lit1);
    scene.SceneBuild();

    Renderer renderer;
    renderer.rendererSet(width, height, pt, camera, 200);
    renderer.Render(scene, "LightTexture_test", sampler);
    return 0;
}