#pragma once
#include <vector>
#include <iostream>
constexpr float PI = 3.14159265359f;
constexpr float PI2 = 2.0f * PI;
constexpr float PI4 = 4.0f * PI;
constexpr float PI_DIV2 = 0.5f * PI;
constexpr float PI_DIV4 = 0.25f * PI;
constexpr float PI_INV = 1.0f / PI;
constexpr float PI_INV2 = 1.0f / PI2;
constexpr float PI_INV4 = 1.0f / PI4;

template <typename T>
std::ostream& operator<<(std::ostream& stream, const std::vector<T>& vec)
{
    for (int i = 0; i < vec.size(); i++) {
        stream << vec[i] << " , ";
    }
    return stream;
}