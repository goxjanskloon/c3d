#pragma once
#include<c3d/hittable.h>
#include<c3d/pdf.h>
namespace c3d{
    struct hit_record;
    struct scatter_record{
        std::shared_ptr<pdf> pdf_;
        float attenuation;
        vector skip_pdf;
    };
    class material{
    public:
        virtual ~material()=0;
        virtual vector emit(const vector &origin,const vector &ray,const hit_record &hit){
            return{.0f,.0f,.0f};
        }
        [[nodiscard]] virtual std::shared_ptr<scatter_record> scatter(const hit_record &hit)const=0;
    };
}