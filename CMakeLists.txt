cmake_minimum_required(VERSION 3.12)
project(main LANGUAGES C CXX)

# set(CMAKE_MODULE_PATH '../external/embree-3.13.3.x86_64.linux/lib/cmake/embree-3.13.3')

find_package(embree 3.0 REQUIRED)
find_package(OpenMP)

add_executable(main "src/RendererTest.cpp")
target_include_directories(main PRIVATE "include")
target_include_directories(main PRIVATE "external")
target_link_libraries(main embree)
target_link_libraries(main OpenMP::OpenMP_CXX)
target_compile_features(main PRIVATE cxx_std_17)