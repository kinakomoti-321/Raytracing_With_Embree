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
public:
    std::shared_ptr<Image> image;
    string name;

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

        for (int j = 0; j < width; j++)
        {
            for (int i = 0; i < height; i++)
            {

                constexpr float divider = 1.0f / 255.0f;
                const float R = img[3 * j + 3 * width * i] * divider;
                const float G = img[3 * j + 3 * width * i + 1] * divider;
                const float B = img[3 * j + 3 * width * i + 2] * divider;
                image->setPixel(i, j, Vec3(R, G, B));
            }
        }

        name = filename;
    }

    Vec3 getTex(float u, float v)
    {
        u = std::clamp(std::fmod(u, 1.0f), 0.0f, 1.0f);
        v = std::clamp(std::fmod(v, 1.0f), 0.0f, 1.0f);

        unsigned int u1 = floor(u * image->getWidth());
        unsigned int v1 = floor(v * image->getHeight());
        return image->getPixel(u1, v1);
    }
};

class WorldTexture
{
public:
    std::shared_ptr<Image> image;
    string name;

    WorldTexture(const string& filename)
    {
        cout << "Texture Loading :" << filename << endl;
        int width, height, channels;
        float* img = stbi_loadf(filename.c_str(), &width, &height, &channels, 0);
        if (img == NULL)
        {
            cout << "Texture " << filename << " Load Failed" << endl;
            image = std::make_shared<Image>(1, 1);
            return;
        }
        image = std::make_shared<Image>(width, height);
        cout << width << " " << height << endl;
        cout << "Texture loading" << endl;

        for (int j = 0; j < width; j++)
        {
            for (int i = 0; i < height; i++)
            {
                int index = 3 * j + 3 * width * i;

                constexpr float divider = 1.0f / 255.0f;
                const float R = img[3 * j + 3 * width * i];
                const float G = img[3 * j + 3 * width * i + 1];
                const float B = img[3 * j + 3 * width * i + 2];
                image->setPixel(j, i, Vec3(R, G, B));
            }
        }
        cout << "Texture " << filename << " Load Finished" << endl;
        name = filename;
    }

    Vec3 getTex(float u, float v)
    {
        u = std::clamp(std::fmod(u, 1.0f), 0.0f, 1.0f);
        v = std::clamp(std::fmod(v, 1.0f), 0.0f, 1.0f);
        unsigned int u1 = floor(u * image->getWidth());
        unsigned int v1 = floor(v * image->getHeight());
        return image->getPixel(u1, v1);
    }
};
#endif