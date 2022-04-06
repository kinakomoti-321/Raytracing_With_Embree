#include <embree3/rtcore.h>
#include <limits>
#include <iostream>
#include "core/ray.h"
#include "math/vec3.h"
#include "scene/scene.h"
#include "image/image.h"
#include "camera/pinholecamera.h"
int main()
{
    const unsigned int width = 512,height = 512;
    Image img(width,height);
    
    Ray ray;
    ray.origin = Vec3(0,0,-1);
    ray.direction = normalize(Vec3(0,0,1));
    Scene scene;
    scene.addPolygon();
    scene.SceneBuild();
    IntersectInfo info;
    info = scene.Intersection(ray,info);
    
    Vec3 cameraPos(0,0,-3);
    Vec3 cameraDir(0,0,1);
    PinholeCamera camera(cameraPos,cameraDir,1.0f);
    

    for(int j = 0; j < height; j++){
        for(int i = 0; i < width; i++){
            float u = 2.0f * float(i) / width - 1.0;
            float v = 2.0f * float(j) / width - 1.0;

            Ray ray = camera.getCameraRay(u,v);
            Vec3 col;
            IntersectInfo info;
            scene.Intersection(ray,info);

            if(info.hit) col = Vec3(1.0);

            img.setPixel(i,j,col);
        }
    }
  
    img.writePNG("output");
    return 0;
}