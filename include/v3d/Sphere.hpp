#pragma once
#include<v3d/definitions.hpp>
#include<v3d/Renderable.hpp>
#include<v3d/Ray.hpp>
#include<v3d/Vector.hpp>
namespace v3d{
    class Sphere:public Renderable{
        public:
            Vector center;
            Real radius,roughn;
            Color color,light;
            Sphere(CR<Vector> center,CR<Real> radius,CR<Color> color,CR<Color> light,CR<Real> roughn):center(center),radius(radius),color(color),light(light),roughn(roughn){}
            Ptr<Point> pick(CR<Ray> ray)const override{
                const auto cp=(ray.pos-center);
                const Real b=ray.ray*cp,d=b*b-cp*cp+radius*radius;
                if(d<0) return nullptr;
                Real t=-b-sqrt(d);
                if(t<EPSILON) t+=sqrt(d);
                if(t>EPSILON) return makePtr<Point>(ray.at(t),(cp+ray.ray*t).unit(),color,light,t,roughn);
                return nullptr;
            }
    };
}