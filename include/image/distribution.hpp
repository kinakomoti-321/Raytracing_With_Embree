#pragma once
#include <vector>
#include <memory>
#include "math/vec2.h"
#include "image.h"

//refered to https://pbr-book.org/3ed-2018/Monte_Carlo_Integration/Sampling_Random_Variables#Distribution1D::SampleContinuous
struct Distribution1D {
    std::vector<float> func, cdf;
    float funcInt;

    Distribution1D(const std::vector<float>& infunc) {
        unsigned int n = infunc.size();
        func = infunc;
        cdf.push_back(0);
        for (int i = 1; i < n + 1; ++i) {
            cdf.push_back(cdf[i - 1] + func[i - 1] / n);
        }
        funcInt = cdf[n];
        std::cout << "funcInt" << funcInt << std::endl;
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
};

class Distribution2D {
private:
    std::vector<std::unique_ptr<Distribution1D>> pConditionalV;
    std::unique_ptr<Distribution1D> pMargin;

public:
    Distribution2D(const std::vector<float>& data, int nu, int nv) {
        for (int j = 0; j < nv; j++) {
            std::vector<float> func;
            for (int i = 0; i < nu; i++) {
                unsigned int idx = i + j * nu;
                func.push_back(data[i + j * nu]);
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
        unsigned int v = std::clamp(static_cast<int>(uv[1] * pConditionalV[u]->Count()), 0, pConditionalV[u]->Count() - 1);
        // std::cout << pMargin->funcInt << std::endl;
        return pConditionalV[u]->func[v] / pMargin->funcInt;
    }

    void writeTest() const {
        unsigned int width = pMargin->Count(), height = pConditionalV[0]->Count();
        auto image = std::make_shared<Image>(width, height);
        float sum = 0;
        for (int i = 0; i < width; i++) {
            for (int j = 0; j < height; j++) {
                // image->setPixel(i, j, Vec3(1.0) * getPDF(Vec2(i / width, j / height)));
                image->setPixel(i, j, Vec3(1.0) * getPDF(Vec2(float(i) / width, float(j) / height)));
                sum += getPDF(Vec2(float(i) / width, float(j) / height));
            }
        }
        std::cout << "sum" << sum << std::endl;
        image->writePNG("PDF_Distribution");
    }
};