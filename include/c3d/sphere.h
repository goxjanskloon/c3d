#pragma once
#include<c3d/hittable.h>
namespace c3d{
    class sphere final:public hittable{
    public:
        vector center,movement;
        float radius;
        std::shared_ptr<material> material_;
        [[nodiscard]] std::shared_ptr<hit_record> hit(const vector &origin,const vector &ray,float time,const interval &interval_)const override{
            const vector c=center+movement*time,co=origin-c;
            const float b=ray*co,d=b*b-self_dot(co)+radius*radius;
            if(d<0)
                return nullptr;
            const float sd=std::sqrt(d);
            float t=-b-sd;
            if(!interval_.contain(t)){
                t+=sd*2;
                if(!interval_.contain(t))
                    return nullptr;
            }
            const vector p=origin+ray*t;
            return std::make_shared<hit_record>(hit_record{p,unit(p-c),t,(std::atan2(-p.z,p.x)+PI)/(2*PI),std::acos(-p.y)/PI,material_});
        }
        [[nodiscard]] aabb bounding_box()const override{
            aabb aabb_(center,center+movement);
            aabb_.x.expand(radius);
            aabb_.y.expand(radius);
            aabb_.z.expand(radius);
            return aabb_;
        }
    };
}