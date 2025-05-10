#pragma once
#include<c3d/material.h>
#include<c3d/vector.h>
#include<future>
#include<vector>
namespace c3d{
    struct camera{
        int width,height,samples_per_pixel,max_depth,threads;
        vector origin,direction,right,down,background_color;
        std::vector<std::vector<vector>> render(const hittable &world,const hittable &lights){
            const vector c=origin+direction,p0=c-right*(static_cast<float>(width)/2.f)-down*(static_cast<float>(height)/2.f);
            std::vector<std::vector<vector>> img(width);
            std::vector<std::future<void>> ft(threads);
            for(auto i=img.begin();i!=img.end();++i)
                img.emplace(i,height);
            auto color=[&](this auto &&self,const vector &origin,const vector &ray,const float time,const int depth){
                if(depth>max_depth)
                    return vector{0.f,0.f,0.f};
                const auto hit=world.hit(origin,ray,time,hit_interval);
                if(hit==nullptr)
                    return background_color;
                const vector emit=hit->material_->emit(origin,ray,*hit);
                const auto scatter=hit->material_->scatter(*hit);
                if(scatter==nullptr)
                    return emit;
                if(self_dot(scatter->skip_pdf)!=0)
                    return self(hit->point,scatter->skip_pdf,time,depth+1)*scatter->attenuation;
                const mixture_pdf p(std::make_shared<hittable_pdf>(lights,hit->point),scatter->pdf_);
                const vector scattered=p.generate();
                return emit+scatter->attenuation*hit->material_->scattering_pdf(time,*hit,scattered)*self(hit->point,scattered,time,depth+1)/p.value(scattered);
            };
            for(int t=0,d=width/threads;t<threads;++t)
                ft[t]=std::async(std::launch::async,[&](const int m){
                    for(int i=d*m,l=std::min(i+d-1,width);i<l;++i)
                        for(int j=0;j<height;++j){
                            vector p{0.f,0.f,0.f};
                            for(int k=0;k<samples_per_pixel;++k)
                                p+=legal(color(origin,p0+right*(i+random_unit_float()/2)+down*(j+random_unit_float()/2),random_float(),1));
                            img[i][j]=p/samples_per_pixel;
                        }
                },t);
            for(int t=0;t<threads;++t)
                ft[t].get();
            return img;
        }
    };
}