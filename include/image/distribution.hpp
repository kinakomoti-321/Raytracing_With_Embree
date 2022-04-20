#pragma once
#include <vector>
#include <memory>
#include "math/vec2.h"
#include "image.h"
#include "color.hpp"

//refered to https://pbr-book.org/3ed-2018/Monte_Carlo_Integration/Sampling_Random_Variables#Distribution1D::SampleContinuous
struct Distribution1D {
    std::vector<float> func, cdf;
    float funcInt;

    Distribution1D(const std::vector<float>& infunc) {
        unsigned int n = infunc.size();
        func = infunc;

        cdf.push_back(0);
        for (int i = 1; i < n + 1; ++i) {
            cdf.push_back(cdf[i - 1] + func[i - 1] / float(n));

        }

        funcInt = cdf[n];

        // std::cout << "funcInt" << funcInt << std::endl;
        float divFuncInt = 1.0f / funcInt;

        if (funcInt == 0) {
            for (int i = 1; i < n + 1; ++i) {
                cdf[i] = float(i) / float(n);
            }
        }
        else {
            for (int i = 1; i < n + 1; ++i) {
                //cdf normalize
                cdf[i] *= divFuncInt;
            }
        }
    }

    int Count() const { return func.size(); }

    unsigned int getOffset(float u)const {
        //二分探索
        int first = 0, len = cdf.size();

        while (len > 0) {
            int half = len >> 1, middle = first + half;
            if (cdf[middle] <= u) {
                first = middle + 1;
                len -= half + 1;
            }
            else {
                len = half;
            }
        }

        return std::clamp(first - 1, 0, int(cdf.size()) - 2);
    }

    float getSample(const float u, float& pdf, unsigned int& offset) const {
        offset = getOffset(u);
        float du = u - cdf[offset];
        if ((cdf[offset + 1] - cdf[offset]) > 0.0) {
            du /= (cdf[offset + 1] - cdf[offset]);
        }

        pdf = func[offset] / funcInt;

        return (offset + du) / Count();
    }

    float getPDF(unsigned int index) const {
        return func[index] / (funcInt * Count());
    }

    void checkPDF() const {
        float sum;
        for (int i = 0; i < Count(); i++) {
            unsigned int u = std::clamp(static_cast<int>(i), 0, Count() - 1);
            sum += getPDF(u);
        }

        std::cout << sum << std::endl;
    }
};

class Distribution2D {
private:
    std::vector<std::unique_ptr<Distribution1D>> pConditionalV;
    std::unique_ptr<Distribution1D> pMargin;

public:
    Distribution2D(const std::shared_ptr<Image>& data, int nu, int nv, unsigned int mipmapLevel = 5) {
        for (int i = 0; i < nu / mipmapLevel; i++) {
            std::vector<float> func;
            for (int j = 0; j < nv / mipmapLevel; j++) {
                func.push_back(YfromRGB(data->getPixel(i * mipmapLevel, j * mipmapLevel)));
            }
            pConditionalV.push_back(std::make_unique<Distribution1D>(func));
        }
        std::vector<float> mfunc;
        for (int i = 0; i < pConditionalV.size(); i++) {
            mfunc.push_back(pConditionalV[i]->funcInt);
        }
        pMargin = std::make_unique<Distribution1D>(mfunc);
    }

    Vec2 getSample(const Vec2& rnd, float& pdf)const {
        Vec2 uv;
        unsigned int index, dummyv;
        float pdf1, pdf2;
        float u = pMargin->getSample(rnd[0], pdf1, index);
        float v = pConditionalV[index]->getSample(rnd[1], pdf2, dummyv);

        pdf = pdf1 * pdf2;

        return Vec2(u, v);
    }

    float getPDF(const Vec2& uv)const {
        unsigned int u = std::clamp(static_cast<int>(uv[0] * pMargin->Count()), 0, pMargin->Count() - 1);
        // std::cout << "max : " << pMargin->Count() - 1 << std::endl;
        // std::cout << "u " << u << std::endl;
        unsigned int v = std::clamp(static_cast<int>(uv[1] * pConditionalV[u]->Count()), 0, pConditionalV[u]->Count() - 1);
        // std::cout << "max : " << pConditionalV[0]->Count() - 1 << std::endl;
        // std::cout << "v " << v << std::endl;
        return pConditionalV[u]->func[v] / pMargin->funcInt;
        // return pConditionalV[iu]->func[iv] / pMargin->funcInt;
    }

    float getPDF(const unsigned int index1, const unsigned int index2)const {
        return pConditionalV[index1]->func[index2] / pMargin->funcInt;
    }

    float getPDF(const Vec2& uv, const unsigned int index1, const unsigned int index2)const {
        unsigned int u = std::clamp(static_cast<int>(uv[0] * pMargin->Count()), 0, pMargin->Count() - 1);
        // std::cout << "max : " << pMargin->Count() - 1 << std::endl;
        // std::cout << "u " << u << std::endl;
        unsigned int v = std::clamp(static_cast<int>(uv[1] * pConditionalV[u]->Count()), 0, pConditionalV[u]->Count() - 1);
        // std::cout << "max : " << pConditionalV[0]->Count() - 1 << std::endl;
        // std::cout << "v " << v << std::endl;

        // std::cout << "index1 " << index1 << " index2 " << index2 << std::endl;

        // return pConditionalV[index]->func[v] / pMargin->funcInt;
        if (pConditionalV[u]->func[v] / pMargin->funcInt != pConditionalV[index1]->func[index2] / pMargin->funcInt) {
            std::cout << "u " << u << std::endl;
            std::cout << "v " << v << std::endl;

            std::cout << "index1 " << index1 << " index2 " << index2 << std::endl;
            // std::cout << pConditionalV[u]->func[v] / pMargin->funcInt << std::endl;
            // std::cout << pConditionalV[index1]->func[index2] / pMargin->funcInt << std::endl;
        }

        // return pConditionalV[index1]->func[index2] / pMargin->funcInt;
        return pConditionalV[u]->func[v] / pMargin->funcInt;
        // return pConditionalV[iu]->func[iv] / pMargin->funcInt;
    }
    void writeTest() const {
        unsigned int width = pMargin->Count(), height = pConditionalV[0]->Count();
        auto image = std::make_shared<Image>(width, height);
        float sum = 0;
        for (int i = 0; i < width; i++) {
            for (int j = 0; j < height; j++) {
                image->setPixel(i, j, Vec3(1.0) * getPDF(Vec2(float(i) / width, float(j) / height)) / (width * height));
                // sum += getPDF(Vec2(float(i) / width, float(j) / height)) / (width * height);
            }
        }
        std::cout << "sum" << sum << std::endl;
        image->writePNG("PDF_Distribution");
    }

    void checkPDF()const {
        unsigned int width = pMargin->Count(), height = pConditionalV[0]->Count();
        float sum = 0;

        for (int i = 0; i < width; i++) {
            for (int j = 0; j < height; j++) {
                sum += getPDF(Vec2(float(i) / width, float(j) / height), i, j) / (width * height);
            }
        }
        std::cout << "pmfuncInt" << pMargin->funcInt << std::endl;
        std::cout << sum << std::endl;
        // pMargin->checkPDF();
    }
};