#pragma once
#include<c3d/hittable.h>
#include<c3d/onb.h>
namespace c3d{
    class sphere final:public hittable{
    public:
        vector center,movement;
        float radius;
        std::shared_ptr<material> material_;
        [[nodiscard]] std::shared_ptr<hit_record> hit(const vector &origin,const vector &ray,const float time,const interval &interval_)const override{
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
            const vector p=origin+ray*t,on=(p-c)/radius;
            return std::make_shared<hit_record>(p,ray,on,t,(std::atan2(-on.z,on.x)+PI)/(2*PI),std::acos(-on.y)/PI,material_);
        }
        [[nodiscard]] aabb bounding_box()const override{
            aabb aabb_(center,center+movement);
            aabb_.x.expand(radius);
            aabb_.y.expand(radius);
            aabb_.z.expand(radius);
            return aabb_;
        }
        [[nodiscard]] float pdf_value(const vector &origin,const vector &direction)const override{
            if(const auto record=hit(origin,direction,0.f,hit_interval);record==nullptr)
                return 0.f;
            return 1.f/2.f*PI*(1.f-std::sqrt(1.f-radius*radius/self_dot(center-origin)));
        }
        [[nodiscard]] vector random(const vector &origin)const override{
            const vector direction=center-origin;
            const onb uvw(direction);
            const float z=1.f+random_float()*(std::sqrt(1.f-radius*radius/self_dot(direction))-1.f),a=2*PI*random_float(),b=std::sqrt(1-z*z);
            return uvw.transform({std::cos(a)*b,std::sin(a)*b,z});
        }
    };
}