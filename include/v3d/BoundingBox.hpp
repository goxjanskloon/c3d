#pragma once
#include<v3d/definitions.hpp>
#include<v3d/Ray.hpp>
namespace v3d{
    class BoundingBox{
        virtual bool hit(CR<Ray> ray)const=0;
    };
}