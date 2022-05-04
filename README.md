# Raytracing_With_Embree
CPU raytracer using Embree 3.0

![Image1](./picture/GGXtest_1000sample.png)
![Image2](./picture/Glasstest_1000sample.png)
![Image3](./picture/scene1.png)
![Image3](./picture/scene2.png)
![Image3](./picture/scene3.png)
![Image3](./picture/scene4.png)

## Build
This program use Embree 3.0 API. Install Embree from offcial site, and unzip that and put to /external directory.

[Embree](https://www.embree.org/)

```
mkdir build
cd build
cmake ..
make
```

If you can not compile by this command and get the error aboud find_package, change cmake command to 

```
cmake -Dembree_DIR=./external/embree-3.13.3.x86_64.linux/lib/cmake ..
```


## References
- [Embree](https://www.embree.org/)
- [embree-tinyobj-example@yumcyaWiz](https://github.com/yumcyaWiz/embree-tinyobj-example)
- [LTRE@yumcyaWiz](https://github.com/yumcyaWiz/LTRE)
- [Physically Based Rendering](https://www.pbr-book.org/3ed-2018/contents)
- [memoRANDOM@shocker-0x15](https://rayspace.xyz/)