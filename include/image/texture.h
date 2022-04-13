#ifndef TEXTURE_H
#define TEXTURE_H

#define STB_IMAGE_IMPLEMENTATION
#include "../external/stb/stb_image.h"
#include <string>
#include "math/vec3.h"
#include <iostream>
#include "image.h"
#include <memory>
#include <cmath>
#include <algorithm>

using namespace std;

class Texture
{
private:
    Vec2 calcTexCoord(float u, float v) const {
        if (u > 1.0f) {
            u = std::fmod(u, 1.0f);
        }
        else if (u < 0.0) {
            u = 1.0f + std::fmod(u, 1.0f);
        }

        if (v > 1.0f) {
            v = std::fmod(v, 1.0f);
        }
        else if (v < 0.0f) {
            v = 1.0f + std::fmod(v, 1.0f);
        }

        return Vec2(u, v);
    }
public:
    std::shared_ptr<Image> image;
    string name;

    Texture() {
        image = std::make_shared<Image>(1, 1);
        image->setPixel(0, 0, Vec3(0));
    }
    Texture(const Vec3& col) {
        image = std::make_shared<Image>(1, 1);
        image->setPixel(0, 0, col);
    }
    Texture(const string& filename)
    {
        cout << "Texture Loading :" << filename << endl;
        int width, height, channels;
        unsigned char* img = stbi_load(filename.c_str(), &width, &height, &channels, 3);
        if (img == NULL)
        {
            cout << "Texture " << filename << " Load Failed" << endl;
            image = make_shared<Image>(1, 1);
            return;
        }
        image = std::make_shared<Image>(width, height);

        for (int j = 0; j < height; j++)
        {
            for (int i = 0; i < width; i++)
            {

                constexpr float divider = 1.0f / 255.0f;
                const unsigned int idx = i * 3 + 3 * width * j;
                const float R = std::pow(img[idx] * divider, 2.2f);
                const float G = std::pow(img[idx + 1] * divider, 2.2f);
                const float B = std::pow(img[idx + 2] * divider, 2.2f);
                image->setPixel(i, j, Vec3(R, G, B));
            }
        }

        name = filename;
    }

    Vec3 getTex(float u, float v)
    {
        Vec2 uv = calcTexCoord(u, v);
        unsigned int u1 = std::clamp(static_cast<int>(uv[0] * image->getWidth()), 0, int(image->getWidth() - 1));
        unsigned int v1 = std::clamp(static_cast<int>(uv[1] * image->getHeight()), 0, int(image->getHeight() - 1));
        // if (image->getHeight() > 19)std::cout << u1 << "," << v1 << std::endl;
        return image->getPixel(u1, v1);
    }

    void writePNG(std::string filename) {
        image->writePNG(filename);
    }
};

class WorldTexture
{
private:
    Vec2 calcTexCoord(float u, float v) const {
        if (u > 1.0f) {
            u = std::fmod(u, 1.0f);
        }
        else if (u < 0.0) {
            u = 1.0f + std::fmod(u, 1.0f);
        }

        if (v > 1.0f) {
            v = std::fmod(v, 1.0f);
        }
        else if (v < 0.0f) {
            v = 1.0f + std::fmod(v, 1.0f);
        }

        return Vec2(u, v);
    }
public:
    std::shared_ptr<Image> image;
    string name;

    WorldTexture() {
        image = std::make_shared<Image>(1, 1);
        image->setPixel(0, 0, Vec3(0));
    }
    WorldTexture(const Vec3& col) {
        image = std::make_shared<Image>(1, 1);
        image->setPixel(0, 0, col);
    }
    WorldTexture(const string& filename)
    {
        cout << "Texture Loading :" << filename << endl;
        int width, height, channels;
        float* img = stbi_loadf(filename.c_str(), &width, &height, &channels, 3);
        if (img == NULL)
        {
            cout << "Texture " << filename << " Load Failed" << endl;
            image = make_shared<Image>(1, 1);
            return;
        }
        image = std::make_shared<Image>(width, height);

        for (int j = 0; j < height; j++)
        {
            for (int i = 0; i < width; i++)
            {

                constexpr float divider = 1.0f / 255.0f;
                const unsigned int idx = i * 3 + 3 * width * j;
                const float R = img[idx];
                const float G = img[idx + 1];
                const float B = img[idx + 2];
                image->setPixel(i, j, Vec3(R, G, B));
            }
        }

        name = filename;
    }

    Vec3 getTex(float u, float v)
    {
        Vec2 uv = calcTexCoord(u, v);
        unsigned int u1 = std::clamp(static_cast<int>(uv[0] * image->getWidth()), 0, int(image->getWidth() - 1));
        unsigned int v1 = std::clamp(static_cast<int>(uv[1] * image->getHeight()), 0, int(image->getHeight() - 1));

        return image->getPixel(u1, v1);
    }

    void writePNG(std::string filename) {
        image->writePNG(filename);
    }
};
#endif