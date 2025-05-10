#pragma once
#include<c3d/aabb.h>
#include<c3d/vector.h>
namespace c3d{
    class material;
    struct hit_record{
        vector point,normal;
        float distance,u,v;
        bool front_face;
        std::shared_ptr<material> material_;
        hit_record(const vector &point,const vector &ray,const vector &outward_normal,const float distance,const float u,const float v,const std::shared_ptr<material> &material_):point(point),distance(distance),u(u),v(v),material_(material_){
            normal=((front_face=ray*outward_normal<0))?outward_normal:-outward_normal;
        }
    };
    class hittable{
    public:
        virtual ~hittable()=0;
        [[nodiscard]] virtual std::shared_ptr<hit_record> hit(const vector &origin,const vector &ray,float time,const interval &interval_)const=0;
        [[nodiscard]] virtual aabb bounding_box()const=0;
        [[nodiscard]] virtual float pdf_value(const vector &origin,const vector &ray)const=0;
        [[nodiscard]] virtual vector random(const vector &origin)const=0;
    };
    static constexpr interval hit_interval{1e-4,INF};
}