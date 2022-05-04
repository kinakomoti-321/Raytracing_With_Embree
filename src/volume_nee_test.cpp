#include "renderer/renderer.hpp"
#include "camera/camera.hpp"
#include "camera/pinholecamera.h"
#include "bsdf/bsdf.hpp"
#include "bsdf/lambert.hpp"
#include "integrator/integrator.hpp"
#include "integrator/normalchecker.hpp"
#include "integrator/volume_homo_nee.hpp"
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
    // const unsigned int width = 512 * 2, height = 512 * 1.5;
    const unsigned int width = 512, height = 512;
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

    auto worldtex = std::make_shared<WorldTexture>("../texture/TestHDR1.hdr");
    // auto worldtex = std::make_shared<WorldTexture>(Vec3(1.0));
    auto mat1 = std::make_shared<Diffuse>(Vec3(0.8));
    auto mat2 = std::make_shared<Diffuse>(Vec3(0.9, 0.2, 0.2));
    auto mat3 = std::make_shared<Diffuse>(Vec3(0.2, 0.9, 0.2));
    auto mat4 = std::make_shared<Diffuse>(tex1);
    auto mat5 = std::make_shared<IdealGlass>(tex3, tex2);
    auto mat6 = std::make_shared<Metallic>(tex3, tex4, tex41, tex42);
    auto mat7 = std::make_shared<Mirror>(tex3);
    auto mat8 = std::make_shared<Diffuse>(tex6);
    auto mat9 = std::make_shared<Diffuse>(tex7);
    auto mat10 = std::make_shared<Diffuse>(tex8);

    auto lit1 = std::make_shared<Light>(Vec3(1.0) * 3.0);

    Vec3 cameraPos(-1.0, 0.0, -1.0);
    // cameraPos *= 3.55;
    Vec3 cameraDir = normalize(Vec3(0, 0.0, 0) - cameraPos);


    auto camera = std::make_shared<PinholeCamera>(cameraPos, cameraDir, 2.0f);
    auto camera1 = std::make_shared<ThinLens>(cameraPos, cameraDir, 1, PI / 4.0, 55.0);
    camera1->forcus(Vec3(-0.5, 1, 0));
    auto integrator = std::make_shared<MIS>();
    auto integrator1 = std::make_shared<NEE>();
    auto integrator2 = std::make_shared<PathTracer>();
    auto integrator3 = std::make_shared<Volume_homo_render>();
    auto integrator4 = std::make_shared<UVChecker>();
    auto integrator5 = std::make_shared<TextureChecker>();
    auto integrator6 = std::make_shared<VolumeNEE>();

    auto vol = std::make_shared<HomoVolume>(10.0, 10.0, -0.8, Vec3(0));

    auto sampler = std::make_shared<RNGrandom>();
    Scene scene;
    // scene.setSkySphere(worldtex, 1.0);
    scene.setSkySphere(Vec3(0.001));

    // scene.setSkyDirectionalLight(normalize(Vec3(1, 1, -1)), Vec3(2.0));
    scene.addPolygon("../model/dragon.obj", mat6, nullptr, vol);
    // scene.addPolygon("../model/buddha.obj", mat1, nullptr, vol);
    // scene.addPolygon("../model/floor.obj", mat10);
    // scene.addPolygon("../model/UVBox.obj", mat5);
    scene.addPolygon("../model/cornel_L.obj", mat3);
    scene.addPolygon("../model/cornel_R.obj", mat2);
    scene.addPolygon("../model/cornelBox.obj", mat1);
    scene.addPolygon("../model/Light.obj", mat1, lit1);
    // scene.addPolygon("../model/CornellBox-Empty-CO.obj", mat1);
    scene.SceneBuild();

    Renderer renderer;
    renderer.rendererSet(width, height, integrator3, camera, 100);
    renderer.Render(scene, "Volume_nee_reference2", sampler);

    // renderer.TimeLimitRender(scene, "MIS-Sample1", sampler, 60000);
    // renderer.rendererSet(width, height, integrator2, camera, 100);
    // renderer.TimeLimitRender(scene, "IBL-PTtest", sampler, 60000);
    // renderer.rendererSet(width, height, integrator1, camera, 100);
    // renderer.TimeLimitRender(scene, "IBL-NEEtest", sampler, 60000);
    return 0;
}