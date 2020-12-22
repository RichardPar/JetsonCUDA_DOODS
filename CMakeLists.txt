# require CMake 2.8 or greater
cmake_minimum_required(VERSION 2.8)

# declare project
project(cnnserver)

find_package(Pistache 0.0.2 REQUIRED)
find_package(jetson-utils)
find_package(jetson-inference)

find_package(CUDA)

cuda_add_executable(cnnserver main.cpp base64.cpp)

target_link_libraries(cnnserver jetson-inference pthread pistache_shared)

include_directories("c/")

