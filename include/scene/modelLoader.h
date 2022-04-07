#pragma once 
#include <string>
#include <vector>
#include <iostream>
#define TINYOBJLOADER_IMPLEMENTATION
#include "../external/tinyobjloader/tiny_obj_loader.h"


bool loadObj(const std::string& filename, std::vector<float>& vertices,
    std::vector<unsigned int>& indices, std::vector<float>& normals,
    std::vector<float>& uvs)
{
    tinyobj::ObjReader reader;
    std::cout << std::endl << "-------------------" << std::endl;
    std::cout << filename << " load start" << std::endl;
    std::cout << "-------------------" << std::endl;

    if (!reader.ParseFromFile(filename))
    {
        if (!reader.Error().empty())
        {
            std::cerr << reader.Error();
        }
        return false;
    }

    if (!reader.Warning().empty())
    {

        std::cout << std::endl << "...loading error" << std::endl;
        std::cout << reader.Warning();

    }

    const auto& attrib = reader.GetAttrib();
    const auto& shapes = reader.GetShapes();

    //シェイプ数分のループ
    for (size_t s = 0; s < shapes.size(); s++) {
        size_t index_offset = 0;
        //シェイプのフェイス分のループ
        for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {

            //シェイプsの面fに含まれる頂点数
            size_t fv = size_t(shapes[s].mesh.num_face_vertices[f]);

            for (size_t v = 0; v < fv; v++) {
                tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
                //シェイプのv番目の頂点座標
                tinyobj::real_t vx = attrib.vertices[3 * size_t(idx.vertex_index) + 0];
                tinyobj::real_t vy = attrib.vertices[3 * size_t(idx.vertex_index) + 1];
                tinyobj::real_t vz = attrib.vertices[3 * size_t(idx.vertex_index) + 2];

                vertices.push_back(vx);
                vertices.push_back(vy);
                vertices.push_back(vz);

                //シェイプのv番目の法線ベクトル
                tinyobj::real_t nx = attrib.normals[3 * size_t(idx.normal_index) + 0];
                tinyobj::real_t ny = attrib.normals[3 * size_t(idx.normal_index) + 1];
                tinyobj::real_t nz = attrib.normals[3 * size_t(idx.normal_index) + 2];

                normals.push_back(nx);
                normals.push_back(ny);
                normals.push_back(nz);

                //シェイプのv番目のUV
                tinyobj::real_t tx = attrib.texcoords[3 * size_t(idx.texcoord_index) + 0];
                tinyobj::real_t ty = attrib.texcoords[3 * size_t(idx.texcoord_index) + 1];

                uvs.push_back(tx);
                uvs.push_back(ty);

                //v番目のindex
                indices.push_back(index_offset + f);
            }
            index_offset += fv;
        }
    }
    std::cout << std::endl << "...model information" << std::endl;
    std::cout << "the number of vertex : " << vertices.size() / 3 << std::endl;
    std::cout << "the number of normal : " << normals.size() / 3 << std::endl;
    std::cout << "the number of texcoord : " << uvs.size() / 2 << std::endl;
    std::cout << "the number of face : " << indices.size() / 3 << std::endl;
    return true;
}
