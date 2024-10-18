#pragma once
#include<v3d/definitions.hpp>
#include<v3d/Ray.hpp>
namespace v3d{
    class Aabb{
    public:
        Vector l,r;
        static const Aabb empty;
        Aabb():Aabb(empty){}
        Aabb(CR<Vector> l,CR<Vector> r):l(l),r(r){}
        Aabb(CR<Aabb> b1,CR<Aabb> b2):l(std::min(b1.l.x,b2.l.x),std::min(b1.l.y,b2.l.y),std::min(b1.l.z,b2.l.z)),r(std::max(b1.r.x,b2.r.x),std::max(b1.r.y,b2.r.y),std::max(b1.r.z,b2.r.z)){}
        bool hit(CR<Ray> ray)const{
            const auto tl=(l-ray.pos)/ray.ray,tr=(r-ray.pos);
            return std::max(tl.x,tr.x)>EPSILON&&std::max(tl.y,tr.y)>EPSILON&&std::max(tl.z,tr.z)>EPSILON;
        }
        char getLongestAxis()const{
            const auto [x,y,z]=r-l;
            if(x>y&&x>z) return 'X';
            if(y>x&&y>z) return 'Y';
            return 'Z';
        }
        static bool compareX(CR<Aabb> b1,CR<Aabb> b2){return b1.l.x<b2.l.x;}
        static bool compareY(CR<Aabb> b1,CR<Aabb> b2){return b1.l.y<b2.l.y;}
        static bool compareZ(CR<Aabb> b1,CR<Aabb> b2){return b1.l.z<b2.l.z;}
    };
    inline const Aabb Aabb::empty{{REAL_MAX,REAL_MAX,REAL_MAX},{-REAL_MAX,-REAL_MAX,-REAL_MAX}};
}