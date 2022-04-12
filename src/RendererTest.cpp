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

    auto mat1 = std::make_shared<Diffuse>(Vec3(0.9));
    auto mat2 = std::make_shared<Diffuse>(Vec3(0.9, 0.2, 0.2));
    auto mat3 = std::make_shared<Diffuse>(Vec3(0.2, 0.9, 0.2));
    auto mat4 = std::make_shared<Diffuse>(tex1);

    auto lit1 = std::make_shared<Light>(Vec3(1.0) * 3.0);

    Vec3 cameraPos(0, 0, -3);
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
    // scene.addPolygon("../model/dragon.obj", mat7, nullptr, vol);
    scene.addPolygon("../model/UVBox.obj", mat4);
    scene.addPolygon("../model/cornel_L.obj", mat3);
    scene.addPolygon("../model/cornel_R.obj", mat2);
    scene.addPolygon("../model/cornelBox.obj", mat1);
    scene.addPolygon("../model/Light.obj", mat1, lit1);
    // scene.addPolygon("../model/CornellBox-Empty-CO.obj", mat1);
    scene.SceneBuild();

    Renderer renderer;
    renderer.rendererSet(width, height, integrator, camera, 100);
    renderer.Render(scene, "Texture_UVcheck", sampler);
    return 0;
}