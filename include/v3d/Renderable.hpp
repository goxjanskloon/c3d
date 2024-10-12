#pragma once
#include<v3d/definitions.hpp>
#include<v3d/Vector.hpp>
namespace v3d{
    class Renderable{
    public:
        class Point{
        public:
            Vector point,normal;
            Color color,light;
            Real dist,roughn;
        };
        virtual ~Renderable()=default;
        virtual Ptr<Point> pick(CR<Ray> ray)const=0;
    };
}