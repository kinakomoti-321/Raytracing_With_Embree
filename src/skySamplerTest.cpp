#include "scene/sky.hpp"
#include "image/texture.h"
#include "sampling/sampler.hpp"
#include "sampling/rng.hpp"
#include <algorithm>
int main() {
    auto skytex = std::make_shared<WorldTexture>("../texture/TestHDR.hdr");
    Sky sky(skytex, 1.0);
    // skytex->writePNG("HDR_reference");
    // skytex->_distribution->writeTest();
    skytex->_distribution->checkPDF();

    auto sampler = std::make_shared<RNGrandom>();
    const unsigned int width = 1024, height = 1024;
    auto image = std::make_shared<Image>(width, height);
    int sampling = 100000;
    for (int i = 0; i < sampling; i++) {
        float pdf;
        Vec2 uv = skytex->getUVsample(Vec2(sampler->getSample(), sampler->getSample()), pdf);
        unsigned int u = std::clamp(static_cast<unsigned int>(uv[0] * width), static_cast <unsigned int>(0), width - 1);
        unsigned int v = std::clamp(static_cast<unsigned int>(uv[1] * height), static_cast <unsigned int>(0), height - 1);

        image->setPixel(u, v, Vec3(1.0, 0, 0));
    }

    image->writePNG("sampling_distribution");
    return 0;
}