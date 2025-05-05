#pragma once
#include<c3d/aabb.h>
#include<c3d/material.h>
#include<c3d/vector.h>
namespace c3d{
    struct hit_record{
        vector point,normal;
        float distance,u,v;
        bool front_face;
        std::shared_ptr<const material> material_;
        void set_face_normal(const vector &ray,const vector &outward_normal){
            normal=((front_face=ray*outward_normal<0))?outward_normal:-outward_normal;
        }
    };
    class hittable{
    public:
        virtual ~hittable()=0;
        [[nodiscard]] virtual std::shared_ptr<hit_record> hit(const vector &origin,const vector &ray,float time,const interval &interval_)const=0;
        [[nodiscard]] virtual aabb bounding_box()const=0;
    };
}