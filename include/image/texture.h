#ifndef TEXTURE_H
#define TEXTURE_H

#define STB_IMAGE_IMPLEMENTATION
#include "ImageLoader.hpp"
#include <string>
#include "../vec3.h"
#include <iostream>
#include "../image.h"
using namespace std;
class Texture
{
public:
    Image* image;
    string name;

    Texture(const string& filename)
    {
        cout << "Texture Loading :" << filename << endl;
        int width, height, channels;
        unsigned char* img = stbi_load(filename.c_str(), &width, &height, &channels, 3);
        if (img == NULL)
        {
            cerr << "テクスチャの読み込みに失敗しました" << endl;
            exit(EXIT_FAILURE);
        }
        image = new Image(width, height);

        for (int j = 0; j < width; j++)
        {
            for (int i = 0; i < height; i++)
            {

                constexpr float divider = 1.0f / 255.0f;
                const float R = img[3 * j + 3 * width * i] * divider;
                const float G = img[3 * j + 3 * width * i + 1] * divider;
                const float B = img[3 * j + 3 * width * i + 2] * divider;
                // cout << R << " " << G << " " << B << endl;
                image->SetPixel(i, j, vec3f(R, G, B));
            }
        }

        name = filename;
    }
    ~Texture()
    {
        delete image;
    }

    vec3f getTex(float u, float v)
    {
        if (u > 1 || u < 0 || v > 1 || v < 0)
            cout << "uv error: not normalized uv" << endl;

        unsigned int u1 = floor(u * image->getWidth());
        unsigned int v1 = floor(v * image->getHeight());
        return image->getPixel(u1, v1);
    }
};

class WorldTexture
{
public:
    Image* image;
    string name;

    WorldTexture(const string& filename)
    {
        cout << "Texture Loading :" << filename << endl;
        int width, height, channels;
        float* img = stbi_loadf(filename.c_str(), &width, &height, &channels, 0);
        if (img == NULL)
        {
            cerr << "テクスチャの読み込みに失敗しました" << endl;
            exit(EXIT_FAILURE);
        }
        image = new Image(width, height);
        cout << width << " " << height << endl;
        cout << "Texture loading" << endl;
        // for (int i = 0; i < width * height * 5; i += 10)
        // {
        //     cout << i << endl;
        // }
        // cout << "end" << endl;

        for (int j = 0; j < width; j++)
        {
            for (int i = 0; i < height; i++)
            {
                // cout << i << " " << j << endl;
                int index = 3 * j + 3 * width * i;
                // cout << index << endl;

                constexpr float divider = 1.0f / 255.0f;
                const float R = img[3 * j + 3 * width * i];
                const float G = img[3 * j + 3 * width * i + 1];
                const float B = img[3 * j + 3 * width * i + 2];
                // cout << R << " " << G << " " << B << endl;
                image->SetPixel(j, i, vec3f(R, G, B));
                // cout << "test" << endl;
            }
        }
        cout << "texture complete" << endl;
        name = filename;
    }
    ~WorldTexture()
    {
        delete image;
    }

    vec3f getTex(float u, float v)
    {
        if (u > 1 || u < 0 || v > 1 || v < 0)
        {
            cout << "uv error: not normalized uv" << endl;
            cout << u << " " << v << endl;
            return vec3f(0.0);
        }

        unsigned int u1 = floor(u * (image->getWidth() - 1));
        unsigned int v1 = floor(v * (image->getHeight() - 1));
        return image->getPixel(u1, v1);
    }
};
#endif