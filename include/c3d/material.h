#pragma once
#include<c3d/hittable.h>
#include<c3d/pdf.h>
#include<c3d/texture.h>
namespace c3d{
    struct scatter_record{
        std::shared_ptr<pdf> pdf_;
        vector attenuation,skip_pdf;
    };
    class material{
    public:
        virtual ~material()=0;
        [[nodiscard]] virtual vector emit(const vector &origin,const vector &ray,const hit_record &hit)const{
            return{0.f,0.f,0.f};
        }
        [[nodiscard]] virtual std::shared_ptr<scatter_record> scatter(const hit_record &hit)const{
            return nullptr;
        }
        [[nodiscard]] virtual float scattering_pdf(float time,const hit_record &hit,const vector &scattered)const{
            return 0.f;
        }
    };
    class lambertian final:public material{
    public:
        std::shared_ptr<texture> texture_;
        [[nodiscard]] std::shared_ptr<scatter_record> scatter(const hit_record &hit)const override{
            return std::make_shared<scatter_record>(std::make_shared<cosine_pdf>(hit.normal),texture_->value(hit.u,hit.v,hit.point),vector{0.f,0.f,0.f});
        }
        [[nodiscard]] float scattering_pdf(float time,const hit_record &hit,const vector &scattered)const override{
            const float a=hit.normal*scattered;
            return a<0?0:a/PI;
        }
    };
    class diffuse_light final:public material{
    public:
        std::shared_ptr<texture> texture_;
        [[nodiscard]] vector emit(const vector &origin,const vector &ray,const hit_record &hit)const override{
            if (!hit.front_face)
                return{0.f,0.f,0.f};
            return texture_->value(hit.u,hit.v,hit.point);
        }
    };
}
