cmake_minimum_required(VERSION 3.5)
project(v3d-demo-cube)
set(CMAKE_CXX_FLAGS_RELEASE"${CMAKE_CXX_FLAGS_RELEASE} -O3")
add_executable(v3d-demo-cube src/main.cpp)