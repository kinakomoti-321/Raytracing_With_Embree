#ifndef _VEC2_H
#define _VEC2_H

#include <cassert>
#include <cmath>
#include <iostream>


struct Vec2 {
    float v[2];
    static constexpr unsigned int nComponents = 2;

    explicit constexpr Vec2() : v{ 0, 0 } {}
    explicit constexpr Vec2(float x) : v{ x, x } {}
    explicit constexpr Vec2(float x, float y) : v{ x, y } {}

    explicit constexpr operator bool() const { return v[0] != 0 && v[1] != 0; }
    constexpr bool operator==(const Vec2& v) const {
        return this->v[0] == v[0] && this->v[1] == v[1];
    }

    constexpr const float& operator[](int i) const {
        assert(i >= 0 && i <= 1);
        return v[i];
    }
    constexpr float& operator[](int i) {
        assert(i >= 0 && i <= 1);
        return v[i];
    }

    constexpr Vec2 operator-() const { return Vec2(-v[0], -v[1]); }

    constexpr Vec2& operator+=(const Vec2& v) {
        for (int i = 0; i < 2; ++i) {
            this->v[i] += v[i];
        }
        return *this;
    }
    constexpr Vec2& operator-=(const Vec2& v) {
        for (int i = 0; i < 2; ++i) {
            this->v[i] -= v[i];
        }
        return *this;
    }
    constexpr Vec2& operator*=(const Vec2& v) {
        for (int i = 0; i < 2; ++i) {
            this->v[i] *= v[i];
        }
        return *this;
    }
    constexpr Vec2& operator/=(const Vec2& v) {
        for (int i = 0; i < 2; ++i) {
            this->v[i] /= v[i];
        }
        return *this;
    }

    std::string toString() const {
        return "(" + std::to_string(v[0]) + ", " + std::to_string(v[1]) + ")";
    }

    bool isNan() const { return std::isnan(v[0]) || std::isnan(v[1]); }
};

inline constexpr Vec2 operator+(const Vec2& v1, const Vec2& v2) {
    Vec2 ret;
    for (int i = 0; i < 2; ++i) {
        ret[i] = v1[i] + v2[i];
    }
    return ret;
}
inline constexpr Vec2 operator+(const Vec2& v1, float k) {
    Vec2 ret;
    for (int i = 0; i < 2; ++i) {
        ret[i] = v1[i] + k;
    }
    return ret;
}
inline constexpr Vec2 operator+(float k, const Vec2& v2) {
    Vec2 ret;
    for (int i = 0; i < 2; ++i) {
        ret[i] = k + v2[i];
    }
    return ret;
}

inline constexpr Vec2 operator-(const Vec2& v1, const Vec2& v2) {
    Vec2 ret;
    for (int i = 0; i < 2; ++i) {
        ret[i] = v1[i] - v2[i];
    }
    return ret;
}
inline constexpr Vec2 operator-(const Vec2& v1, float k) {
    Vec2 ret;
    for (int i = 0; i < 2; ++i) {
        ret[i] = v1[i] - k;
    }
    return ret;
}
inline constexpr Vec2 operator-(float k, const Vec2& v2) {
    Vec2 ret;
    for (int i = 0; i < 2; ++i) {
        ret[i] = k - v2[i];
    }
    return ret;
}

inline constexpr Vec2 operator*(const Vec2& v1, const Vec2& v2) {
    Vec2 ret;
    for (int i = 0; i < 2; ++i) {
        ret[i] = v1[i] * v2[i];
    }
    return ret;
}
inline constexpr Vec2 operator*(const Vec2& v1, float k) {
    Vec2 ret;
    for (int i = 0; i < 2; ++i) {
        ret[i] = v1[i] * k;
    }
    return ret;
}
inline constexpr Vec2 operator*(float k, const Vec2& v2) {
    Vec2 ret;
    for (int i = 0; i < 2; ++i) {
        ret[i] = k * v2[i];
    }
    return ret;
}

inline constexpr Vec2 operator/(const Vec2& v1, const Vec2& v2) {
    Vec2 ret;
    for (int i = 0; i < 2; ++i) {
        ret[i] = v1[i] / v2[i];
    }
    return ret;
}
inline constexpr Vec2 operator/(const Vec2& v1, float k) {
    Vec2 ret;
    for (int i = 0; i < 2; ++i) {
        ret[i] = v1[i] / k;
    }
    return ret;
}
inline constexpr Vec2 operator/(float k, const Vec2& v2) {
    Vec2 ret;
    for (int i = 0; i < 2; ++i) {
        ret[i] = k / v2[i];
    }
    return ret;
}

inline constexpr float dot(const Vec2& v1, const Vec2& v2) {
    float ret = 0;
    for (int i = 0; i < 2; ++i) {
        ret += v1[i] * v2[i];
    }
    return ret;
}

inline constexpr float length(const Vec2& v) { return std::sqrt(dot(v, v)); }
inline constexpr float length2(const Vec2& v) { return dot(v, v); }

inline std::ostream& operator<<(std::ostream& stream, const Vec2& v) {
    stream << "(" << v[0] << ", " << v[1] << ")";
    return stream;
}
#endif