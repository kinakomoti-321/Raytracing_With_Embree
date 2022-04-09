#pragma once 
#include "modelLoader.h"
#include <vector>
#include "bsdf/bsdf.hpp"
#include <string>
#include <memory>
#include <iostream>
#include "math/vec3.h"
#include "math/vec2.h"
#include "core/constant.hpp"
#include "bsdf/light.hpp"
#include <embree3/rtcore.h>

class Polygon {
private:
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    std::vector<float> normals;
    std::vector<float> texcoords;
    std::vector<std::shared_ptr<BSDF>> material;
    std::vector<std::shared_ptr<Light>> light;
    std::vector<unsigned int> lightFaceID;

    unsigned int nPoly;

public:
    Polygon() {
        nPoly = 0;
    }

    void AddPolygon(const std::string& filepath, const std::shared_ptr<BSDF>& mat, const std::shared_ptr<Light>& lit = nullptr) {
        std::vector<float> vert;
        std::vector<unsigned int> index;
        std::vector<float> nor;
        std::vector<float> uv;
        if (!loadObj(filepath, vert, index, nor, uv)) {
            std::cerr << "can't load object : " << filepath << std::endl;
            return;
        }
        else {}

        std::cout << std::endl << "Loading" << std::endl;
        unsigned int nvert = vert.size();
        unsigned int nowvert = vertices.size() / 3; //現在のverticesの数

        for (int i = 0; i < nvert; i++) {
            vertices.push_back(vert[i]);
            normals.push_back(nor[i]);
        }
        std::cout << "vertex and normal loaded" << std::endl;

        for (int i = 0; i < index.size(); i++) {
            indices.push_back(index[i] + nowvert);
        }
        std::cout << "index loaded" << std::endl;

        for (int i = 0; i < uv.size(); i++) {
            texcoords.push_back(uv[i]);
        }

        std::cout << "texcoord loaded" << std::endl;

        for (int i = 0; i < index.size() / 3; i++) {
            material.push_back(mat);
            light.push_back(lit);
            if (lit != nullptr)lightFaceID.push_back(nPoly + i);
        }
        std::cout << "material loaded" << std::endl;

        std::cout << std::endl << "FaceID : " << nPoly << " ~ ";
        nPoly = indices.size() / 3;
        std::cout << nPoly - 1 << std::endl;

        std::cout << lightFaceID << std::endl;

        std::cout << "model road " << filepath << " complete" << std::endl << "the number Polygon of this model is " << nPoly << std::endl;
        std::cout << std::endl << "-------------------" << std::endl;
        std::cout << filepath << " load end" << std::endl;
        std::cout << "-------------------" << std::endl;
    }

    unsigned int nFace() const {
        return nPoly;
    }
    unsigned int nvertices() const {
        return vertices.size() / 3;
    }

    struct VertexIndex
    {
        unsigned int idx1;
        unsigned int idx2;
        unsigned int idx3;
    };

    VertexIndex getindices(const unsigned int FaceID) const {
        unsigned int id = FaceID * 3;
        VertexIndex index;
        index.idx1 = indices[id + 0];
        index.idx2 = indices[id + 1];
        index.idx3 = indices[id + 2];
        return index;
    }

    Vec3 getvertex(unsigned int index) const {
        Vec3 vert;
        unsigned int id = index * 3;
        vert[0] = vertices[id + 0];
        vert[1] = vertices[id + 1];
        vert[2] = vertices[id + 2];
        return vert;
    }

    Vec3 getVertnormal(const unsigned int index) const {
        Vec3 normal;
        unsigned id = index * 3;
        normal[0] = normals[id + 0];
        normal[1] = normals[id + 1];
        normal[2] = normals[id + 2];
        return normal;
    }

    Vec2 getVertTexcoord(const unsigned int index) const {
        Vec2 uv;
        unsigned id = index * 2;
        // std::cout << "test1" << id << std::endl;
        uv[0] = texcoords[id + 0];
        // std::cout << "test1" << std::endl;
        uv[1] = texcoords[id + 1];
        // std::cout << "test1" << std::endl;
        return uv;
    }

    Vec3 getFaceNormal(const unsigned int FaceID, Vec2 barycentric) const {
        VertexIndex index = this->getindices(FaceID);
        Vec3 n1 = getVertnormal(index.idx1);
        Vec3 n2 = getVertnormal(index.idx2);
        Vec3 n3 = getVertnormal(index.idx3);

        //重心座標系
        return n1 * (1.0f - barycentric[0] - barycentric[1]) + n2 * barycentric[0] + n3 * barycentric[1];
    }

    Vec2 getFaceTexcoord(const unsigned int FaceID, Vec2 barycentric) const {
        // std::cout << "tex" << std::endl;
        VertexIndex index = this->getindices(FaceID);
        // std::cout << "index" << index.idx1 << index.idx2 << index.idx3 << std::endl;
        Vec2 uv1 = getVertTexcoord(index.idx1);
        // std::cout << "tex2" << std::endl;
        Vec2 uv2 = getVertTexcoord(index.idx2);
        // std::cout << "tex2" << std::endl;
        Vec2 uv3 = getVertTexcoord(index.idx3);
        //重心座標系
        return uv1 * (1.0f - barycentric[0] - barycentric[1]) + uv2 * barycentric[0] + uv3 * barycentric[1];
    }

    float getTriangleArea(unsigned int FaceID)const {
        VertexIndex index = this->getindices(FaceID);
        Vec3 v1 = getvertex(index.idx1);
        Vec3 v2 = getvertex(index.idx2);
        Vec3 v3 = getvertex(index.idx3);

        Vec3 va1 = v2 - v1;
        Vec3 va2 = v3 - v1;

        return std::abs(norm(cross(va1, va2))) / 2.0f;
    }

    void sampleLightPoint(float& pdf, const std::shared_ptr<Sampler>& sampler, IntersectInfo& info)const {
        int nlit = lightFaceID.size();
        unsigned int lightID = nlit * sampler->getSample();
        if (lightID == nlit) lightID--;

        unsigned int faceID = lightFaceID[lightID];
        pdf = 1.0f / static_cast<float>(getTriangleArea(faceID) * nlit);

        areaSampling(faceID, sampler, info);
        info.FaceID = faceID;
    }

    float lightPointPDF(unsigned int FaceID, const Vec3& lightPos) const {
        return 1.0f / static_cast<float>(getTriangleArea(FaceID) * lightFaceID.size());
    }

    void areaSampling(unsigned int FaceID, const std::shared_ptr<Sampler>& sampler, IntersectInfo& info)const {
        VertexIndex index = getindices(FaceID);
        Vec3 v1 = getvertex(index.idx1);
        Vec3 v2 = getvertex(index.idx2);
        Vec3 v3 = getvertex(index.idx3);

        Vec3 n1 = getVertnormal(index.idx1);
        Vec3 n2 = getVertnormal(index.idx2);
        Vec3 n3 = getVertnormal(index.idx3);

        Vec2 uv1 = getVertTexcoord(index.idx1);
        Vec2 uv2 = getVertTexcoord(index.idx2);
        Vec2 uv3 = getVertTexcoord(index.idx3);

        float u = sampler->getSample();
        float v = sampler->getSample();

        float f1 = 1.0f - std::sqrt(u);
        float f2 = std::sqrt(u) * (1.0f - v);
        float f3 = std::sqrt(u) * v;

        info.position = v1 * f1 + v2 * f2 + v3 * f3;
        info.normal = n1 * f1 + n2 * f2 + n3 * f3;
        info.texcoord = uv1 * f1 + uv2 * f2 + uv3 * f3;
    }

    std::shared_ptr<BSDF> getMaterial(unsigned int FaceID)const {
        return material[FaceID];
    }
    std::shared_ptr<Light> getLight(unsigned int FaceID)const {
        return light[FaceID];
    }
    bool hasLight(unsigned int FaceID)const {
        return light[FaceID] != nullptr;
    }

    void attachGeometry(RTCGeometry& geom) const {
        unsigned int nvert = vertices.size() / 3;
        float* vb = (float*)rtcSetNewGeometryBuffer(geom,
            RTC_BUFFER_TYPE_VERTEX, 0, RTC_FORMAT_FLOAT3, 3 * sizeof(float), nvert);

        for (int i = 0; i < vertices.size(); i++) {
            vb[i] = vertices[i];
        }

        unsigned* ib = (unsigned*)rtcSetNewGeometryBuffer(geom,
            RTC_BUFFER_TYPE_INDEX, 0, RTC_FORMAT_UINT3, 3 * sizeof(unsigned), nPoly);

        for (int i = 0; i < indices.size(); i++) {
            ib[i] = indices[i];
        }
    }

};