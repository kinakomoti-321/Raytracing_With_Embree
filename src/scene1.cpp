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

    auto cel_colortex = std::make_shared<Texture>("../texture/scene1/celestial_basecolor.png");
    auto cel_roughnessetex = std::make_shared<Texture>("../texture/scene1/celestial_roughness.png");
    auto booktex = std::make_shared<Texture>("../texture/scene1/books.png");
    auto checkertex = std::make_shared<Texture>("../texture/scene1/Checker.png");
    auto tabletex = std::make_shared<Texture>("../texture/scene1/scene1_table.png");

    auto tex1 = std::make_shared<Texture>(Vec3(0.0));

    auto worldtex = std::make_shared<WorldTexture>("../texture/TestHDR2.hdr");

    auto matbook = std::make_shared<Diffuse>(booktex);
    auto matchecker = std::make_shared<Diffuse>(checkertex);
    auto mattable = std::make_shared<Diffuse>(tabletex);
    auto matglass = std::make_shared<IdealGlass>(Vec3(1.0), 1.45f);
    auto matcele = std::make_shared <Metallic>(cel_colortex, cel_roughnessetex, tex1, tex1);

    auto lit1 = std::make_shared<Light>(Vec3(1.0, 0.8, 0.6) * 3.0);
    auto lit2 = std::make_shared<Light>(Vec3(0.52, 0.77, 1.0));

    Vec3 cameraPos(-3.0, 1.0, 5.0);
    Vec3 forcusPoint(0, 1.5, -1.0);
    Vec3 cameraDir = normalize(Vec3(0, 1.5, -1.0) - cameraPos);

    auto camera = std::make_shared<PinholeCamera>(cameraPos, cameraDir, 2.0f);
    auto camera1 = std::make_shared<ThinLens>(cameraPos, cameraDir, 1, PI / 4.0, 55.0);
    camera1->forcus(forcusPoint);
    auto integrator = std::make_shared<MIS>();
    auto sampler = std::make_shared<RNGrandom>();
    Scene scene;
    scene.setSkySphere(worldtex, 1.0f);

    scene.addPolygon("../model/scene1/scene1_books.obj", matbook);
    scene.addPolygon("../model/scene1/scene1_celestial_sphere.obj", matcele);
    scene.addPolygon("../model/scene1/scene1_cloth.obj", matchecker);
    scene.addPolygon("../model/scene1/scene1_sphere.obj", matglass);
    scene.addPolygon("../model/scene1/scene1_table.obj", mattable);
    scene.SceneBuild();

    Renderer renderer;
    renderer.rendererSet(width, height, integrator, camera1, 10);
    renderer.Render(scene, "scene1_lightTest", sampler);

    return 0;
}