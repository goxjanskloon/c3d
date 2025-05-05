#pragma once
#include<c3d/interval.h>
#include<c3d/vector.h>
namespace c3d{
    struct aabb{
        interval x,y,z;
        aabb()=default;
        aabb(const interval &x,const interval &y,const interval &z):x(x),y(y),z(z){}
        aabb(const vector &a,const vector &b):x{std::min(a.x,b.x),std::max(a.x,b.x)},y{std::min(a.y,b.y),std::max(a.y,b.y)},z{std::min(a.z,b.z),std::max(a.z,b.z)}{}
        aabb(const aabb &a,const aabb &b):x(c3d::unite(a.x,b.x)),y(c3d::unite(a.y,b.y)),z(c3d::unite(a.z,b.z)){}
        [[nodiscard]] bool hit(const vector &origin,const vector &ray,interval interval_)const{
            return !(interval_.intersect({(x.min-origin.x)/ray.x,(x.max-origin.x)/ray.x}).empty()
                   ||interval_.intersect({(y.min-origin.y)/ray.y,(y.max-origin.y)/ray.y}).empty()
                   ||interval_.intersect({(z.min-origin.z)/ray.z,(z.max-origin.z)/ray.z}).empty());
        }
        void unite(const aabb &a){
            x.unite(a.x);
            y.unite(a.y);
            z.unite(a.z);
        }
    };
    constexpr aabb empty_aabb{empty_interval,empty_interval,empty_interval};
}