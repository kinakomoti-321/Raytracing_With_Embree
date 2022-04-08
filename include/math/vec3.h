#ifndef _VEC3_H_
#define _VEC3_H_
#include <cmath>
#include <iostream>
struct Vec3 {
  float v[3];
  explicit Vec3() : v{ 0, 0, 0 } {}
  explicit Vec3(float x) : v{ x, x, x } {}
  explicit Vec3(float x, float y, float z) : v{ x, y, z } {}

  float operator[](unsigned int i) const { return v[i]; }
  float& operator[](unsigned int i) { return v[i]; }

  Vec3 operator-() const { return Vec3(-v[0], -v[1], -v[2]); }
  Vec3& operator+=(const Vec3& v) {
    this->v[0] += v[0];
    this->v[1] += v[1];
    this->v[2] += v[2];
    return *this;
  }

  Vec3& operator-=(const Vec3& v) {
    this->v[0] -= v[0];
    this->v[1] -= v[1];
    this->v[2] -= v[2];
    return *this;
  }

  Vec3& operator*=(const Vec3& v) {
    this->v[0] *= v[0];
    this->v[1] *= v[1];
    this->v[2] *= v[2];
    return *this;
  }

  Vec3& operator*=(const float& v) {
    this->v[0] *= v;
    this->v[1] *= v;
    this->v[2] *= v;
    return *this;
  }

  Vec3& operator/=(const Vec3& v) {
    this->v[0] /= v[0];
    this->v[1] /= v[1];
    this->v[2] /= v[2];
    return *this;
  }
  Vec3& operator/=(const float& v) {
    this->v[0] /= v;
    this->v[1] /= v;
    this->v[2] /= v;
    return *this;
  }
};

inline float norm(const Vec3& v) {
  float sum = 0;
  for (int i = 0; i < 3; ++i) {
    sum += v[i] * v[i];
  }

  return std::sqrt(sum);
}

inline float norm2(const Vec3& v) {
  float sum = 0;
  for (int i = 0; i < 3; ++i) {
    sum += v[i] * v[i];
  }

  return sum;
}



inline Vec3 operator+(const Vec3& v1, const Vec3& v2) {
  Vec3 ret;
  for (int i = 0; i < 3; ++i) {
    ret[i] = v1[i] + v2[i];
  }

  return ret;
}

inline Vec3 operator-(const Vec3& v1, const Vec3& v2) {
  Vec3 ret;
  for (int i = 0; i < 3; ++i) {
    ret[i] = v1[i] - v2[i];
  }

  return ret;
}

inline Vec3 operator*(const Vec3& v1, const Vec3& v2) {
  Vec3 ret;
  for (int i = 0; i < 3; ++i) {
    ret[i] = v1[i] * v2[i];
  }

  return ret;
}

inline Vec3 operator/(const Vec3& v1, const Vec3& v2) {
  Vec3 ret;
  for (int i = 0; i < 3; ++i) {
    ret[i] = v1[i] / v2[i];
  }
  return ret;
}

inline Vec3 operator*(const Vec3& v, const float& t) {
  Vec3 ret;
  for (int i = 0; i < 3; ++i) {
    ret[i] = v[i] * t;
  }
  return ret;
}

inline Vec3 operator*(const float& t, const Vec3& v) {
  Vec3 ret;
  for (int i = 0; i < 3; ++i) {
    ret[i] = v[i] * t;
  }
  return ret;
}

inline Vec3 operator/(const Vec3& v, const float& t) {
  Vec3 ret;
  for (int i = 0; i < 3; ++i) {
    ret[i] = v[i] / t;
  }
  return ret;
}

inline Vec3 operator/(const float& t, const Vec3 v) {
  Vec3 ret;
  for (int i = 0; i < 3; ++i) {
    ret[i] = t / v[i];
  }
  return ret;
}

inline float dot(const Vec3& v1, const Vec3& v2) {
  float sum = 0;
  for (int i = 0; i < 3; ++i) {
    sum += v1[i] * v2[i];
  }
  return sum;
}

inline float absdot(const Vec3& v1, const Vec3& v2) {
  float sum = 0;
  for (int i = 0; i < 3; ++i) {
    sum += v1[i] * v2[i];
  }
  return std::abs(sum);
}

inline Vec3 cross(const Vec3& v1, const Vec3& v2) {
  Vec3 ret;
  ret[0] = v1[1] * v2[2] - v1[2] * v2[1];
  ret[1] = v1[2] * v2[0] - v1[0] * v2[2];
  ret[2] = v1[0] * v2[1] - v1[1] * v2[0];

  return ret;
}

inline Vec3 normalize(const Vec3& v) {
  Vec3 ref = v;
  return ref / norm(v);
}

inline Vec3 localToWorld(const Vec3& v, const Vec3& lx, const Vec3& ly,
  const Vec3& lz)
{
  return Vec3(v[0] * lx[0] + v[1] * ly[0] + v[2] * lz[0],
    v[0] * lx[1] + v[1] * ly[1] + v[2] * lz[1],
    v[0] * lx[2] + v[1] * ly[2] + v[2] * lz[2]);
}

inline Vec3 worldtoLocal(const Vec3& v, const Vec3& lx, const Vec3& ly, const Vec3& lz)
{
  return Vec3(v[0] * lx[0] + v[1] * lx[1] + v[2] * lx[2],
    v[0] * ly[0] + v[1] * ly[1] + v[2] * ly[2],
    v[0] * lz[0] + v[1] * lz[1] + v[2] * lz[2]);
}
inline void tangentSpaceBasis(const Vec3& n, Vec3& t, Vec3& b)
{
  if (std::abs(n[1]) < 0.9f)
  {
    t = cross(n, Vec3(0, 1, 0));
  }
  else
  {
    t = cross(n, Vec3(0, 0, -1));
  }
  t = normalize(t);
  b = cross(t, n);
  b = normalize(b);
}

inline Vec3 reflect(const Vec3& v, const Vec3& n) {
  return  -v + 2.0f * dot(v, n) * n;
}

inline std::ostream& operator<<(std::ostream& stream, const Vec3& v) {
  stream << "(" << v[0] << ", " << v[1] << ", " << v[2] << ")";
  return stream;
}

#endif