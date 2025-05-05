cmake_minimum_required(VERSION 3.30)
project(c3d)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
include_directories(${INCLUDE_DIRECTORIES} include)
add_executable(c3d src/main.cc
        include/c3d/c3d.h
        include/c3d/aabb.h
        include/c3d/hittable.h
        include/c3d/pdf.h
        include/c3d/bvh_tree.h
)