cmake_minimum_required(VERSION 3.10)
project(v3d-demo)
set(CMAKE_CXX_FLAGS_RELEASE"${CMAKE_CXX_FLAGS_RELEASE} -O3")
add_executable(v3d-demo src/main.cpp)