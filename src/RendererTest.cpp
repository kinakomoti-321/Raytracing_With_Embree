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
#include <iostream>
#include <memory>

int main() {
    const unsigned int width = 512, height = 512;
    auto tex1 = std::make_shared<Texture>("../texture/UVChecker.jpg");
    auto tex2 = std::make_shared<Texture>(Vec3(1.33));
    auto tex3 = std::make_shared<Texture>(Vec3(1.0));
    auto tex4 = std::make_shared<Texture>(Vec3(0));
    auto tex5 = std::make_shared<Texture>("../texture/Noise.png");
    auto tex6 = std::make_shared<Texture>("../texture/Moon.jpg");
    auto tex7 = std::make_shared<Texture>("../texture/Earth.jpg");
    auto tex8 = std::make_shared<Texture>("../texture/Checker.png");

    auto worldtex = std::make_shared<WorldTexture>("../texture/TestHDR1.hdr");
    // auto worldtex = std::make_shared<WorldTexture>(Vec3(1.0));
    auto mat1 = std::make_shared<Diffuse>(Vec3(0.9));
    auto mat2 = std::make_shared<Diffuse>(Vec3(0.9, 0.2, 0.2));
    auto mat3 = std::make_shared<Diffuse>(Vec3(0.2, 0.9, 0.2));
    auto mat4 = std::make_shared<Diffuse>(tex1);
    auto mat5 = std::make_shared<IdealGlass>(tex3, tex5);
    auto mat6 = std::make_shared<Metallic>(tex3, tex5, tex4, tex4);
    auto mat7 = std::make_shared<Mirror>(tex3);
    auto mat8 = std::make_shared<Diffuse>(tex6);
    auto mat9 = std::make_shared<Diffuse>(tex8);

    auto lit1 = std::make_shared<Light>(Vec3(1.0) * 3.0);

    Vec3 cameraPos(0.0, 0.0, -3.5);
    Vec3 cameraDir = normalize(Vec3(0, 0, 0) - cameraPos);


    auto camera = std::make_shared<PinholeCamera>(cameraPos, cameraDir, 2.0f);

    auto integrator = std::make_shared<MIS>();
    auto integrator1 = std::make_shared<NEE>();
    auto integrator2 = std::make_shared<PathTracer>();
    auto integrator3 = std::make_shared<Volume_homo_render>();
    auto integrator4 = std::make_shared<UVChecker>();
    auto integrator5 = std::make_shared<TextureChecker>();

    auto vol = std::make_shared<HomoVolume>(10.0, 10.0, -0.8, Vec3(0));

    auto sampler = std::make_shared<RNGrandom>();
    Scene scene;
    scene.setSkySphere(worldtex);

    scene.setSkyDirectionalLight(normalize(Vec3(1, 1, -1)), Vec3(2.0));
    scene.addPolygon("../model/dragon.obj", mat7, nullptr, vol);
    // scene.addPolygon("../model/floor.obj", mat9);
    // scene.addPolygon("../model/Sphere1.obj", mat6);
    // scene.addPolygon("../model/Sphere2.obj", mat6);
    // scene.addPolygon("../model/UVBox.obj", mat5);
    scene.addPolygon("../model/cornel_L.obj", mat3);
    scene.addPolygon("../model/cornel_R.obj", mat2);
    scene.addPolygon("../model/cornelBox.obj", mat1);
    // scene.addPolygon("../model/Light.obj", mat1, lit1);
    // scene.addPolygon("../model/CornellBox-Empty-CO.obj", mat1);
    scene.SceneBuild();

    Renderer renderer;
    renderer.rendererSet(width, height, integrator, camera, 300);
    // renderer.Render(scene, "MISreference", sampler);
    renderer.TimeLimitRender(scene, "IBL-MIS-NEEtest", sampler, 240000);
    // renderer.rendererSet(width, height, integrator2, camera, 100);
    // renderer.TimeLimitRender(scene, "IBL-PTtest", sampler, 60000);
    // renderer.rendererSet(width, height, integrator1, camera, 100);
    // renderer.TimeLimitRender(scene, "IBL-NEEtest", sampler, 60000);
    return 0;
}