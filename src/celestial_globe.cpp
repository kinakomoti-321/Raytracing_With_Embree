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
    auto tex1 = std::make_shared<Texture>("../texture/UVChecker.jpg");
    auto tex2 = std::make_shared<Texture>(Vec3(1.43));
    auto tex3 = std::make_shared<Texture>(Vec3(1.0));
    auto tex4 = std::make_shared<Texture>(Vec3(0.3));
    auto tex41 = std::make_shared<Texture>(Vec3(0.0));
    auto tex42 = std::make_shared<Texture>(Vec3(0.0));
    auto tex5 = std::make_shared<Texture>("../texture/Noise.png");
    auto tex6 = std::make_shared<Texture>("../texture/Moon.jpg");
    auto tex7 = std::make_shared<Texture>("../texture/Earth.jpg");
    auto tex8 = std::make_shared<Texture>("../texture/Checker.png");

    auto celestial_col = std::make_shared<Texture>("../texture/test_DefaultMaterial_BaseColor.png");
    auto celestial_rogh = std::make_shared<Texture>("../texture/test_DefaultMaterial_Roughness.png");

    auto worldtex = std::make_shared<WorldTexture>("../texture/TestHDR2.hdr");
    // auto worldtex = std::make_shared<WorldTexture>(Vec3(1.0));
    auto mat1 = std::make_shared<Diffuse>(Vec3(0.8));
    auto mat2 = std::make_shared<Diffuse>(Vec3(0.9, 0.2, 0.2));
    auto mat3 = std::make_shared<Diffuse>(Vec3(0.2, 0.9, 0.2));
    auto mat4 = std::make_shared<Diffuse>(tex8);
    auto mat5 = std::make_shared<IdealGlass>(tex3, tex2);
    auto mat6 = std::make_shared<Metallic>(tex3, tex4, tex41, tex42);
    auto mat7 = std::make_shared<Mirror>(tex3);
    auto mat8 = std::make_shared<Diffuse>(tex6);
    auto mat9 = std::make_shared<Diffuse>(tex7);
    auto mat10 = std::make_shared<Diffuse>(tex8);
    // auto mat11 = std::make_shared<Metallic>(celestial_col, celestial_rogh, tex41, tex42);
    auto mat11 = std::make_shared<Metallic>(celestial_col, celestial_rogh, tex41, tex42);
    auto mat12 = std::make_shared<Diffuse>(celestial_col);

    auto lit1 = std::make_shared<Light>(Vec3(1.0) * 3.0);

    Vec3 cameraPos(0.6, 0.5, -1.0);
    cameraPos *= 2.0;
    Vec3 cameraDir = normalize(Vec3(0, 0.5, 0) - cameraPos);


    auto camera = std::make_shared<PinholeCamera>(cameraPos, cameraDir, 2.0f);
    auto camera1 = std::make_shared<ThinLens>(cameraPos, cameraDir, 1, PI / 4.0, 55.0);
    camera1->forcus(Vec3(-0.5, 1, 0));
    auto integrator = std::make_shared<MIS>();
    auto integrator4 = std::make_shared<DebugPathTracer>();
    auto integrator1 = std::make_shared<UVChecker>();
    auto integrator2 = std::make_shared<NoramlChecker>();
    auto integrator3 = std::make_shared<TextureChecker>();

    auto vol = std::make_shared<HomoVolume>(10.0, 10.0, -0.8, Vec3(0));

    auto sampler = std::make_shared<RNGrandom>();
    Scene scene;
    scene.setSkySphere(worldtex, 1.0);
    // scene.setSkySphere(Vec3(1.0));

    scene.addPolygon("../model/celestial_globe.obj", mat11);
    scene.addPolygon("../model/floor.obj", mat4);
    scene.SceneBuild();

    Renderer renderer;
    renderer.rendererSet(width, height, integrator, camera, 100);
    renderer.Render(scene, "Thin-Lens_Camera_1", sampler);
    return 0;
}