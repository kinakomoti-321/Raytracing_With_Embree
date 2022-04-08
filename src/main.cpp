#include <embree3/rtcore.h>
#include <limits>
#include <memory>
#include <iostream>
#include "core/ray.h"
#include "math/vec3.h"
#include "scene/scene.h"
#include "image/image.h"
#include "camera/pinholecamera.h"
#include "bsdf/bsdf.hpp"
#include "bsdf/lambert.hpp"
#include "bsdf/specular.hpp"
int main()
{
    const unsigned int width = 512, height = 512;
    Image img(width, height);

    std::cout << "polygon" << std::endl;

    auto mat1 = std::make_shared<Lambert>(Vec3(1.0));
    Scene scene;

    scene.addPolygon("../model/dragon.obj", mat1);
    scene.SceneBuild();

    Vec3 cameraPos(1, 0, 0);
    Vec3 cameraDir(-1, 0, 0);
    PinholeCamera camera(cameraPos, cameraDir, 2.0f);


    for (int j = 0; j < height; j++) {
        for (int i = 0; i < width; i++) {
            float u = 2.0f * float(i) / width - 1.0;
            float v = 2.0f * float(j) / width - 1.0;

            Ray ray = camera.getCameraRay(u, v);
            Vec3 col;
            IntersectInfo info;
            if (scene.Intersection(ray, info)) col = info.normal * 0.5 + Vec3(0.5);

            img.setPixel(i, j, col);
        }
    }

    img.writePNG("output");
    return 0;
}