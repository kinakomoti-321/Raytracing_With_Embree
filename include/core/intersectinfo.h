#pragma once 
#include "math/vec3.h" 
#include "math/vec2.h"
struct IntersectInfo {
   Vec3 position;
   Vec3 normal;
   Vec2 texcoord;
   float distance;
   unsigned int FaceID;
};
