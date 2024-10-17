#pragma once
#include<v3d/definitions.hpp>
#include<v3d/Ray.hpp>
#include<v3d/Renderable.hpp>
#include<v3d/Vector.hpp>
namespace v3d{
    class Renderer:public List<Ptr<const Renderable>>{
    public:
        mutable RandomEngine engine{RandomDevice{}()};
        mutable RealDist dist{-0.5,0.5};
        Vector ud,rd;
        Ray ray;
        Int width,height,samples;
        Color bgcolor;
        Renderer(CR<Ray> ray,CR<Vector> ud,CR<Vector> rd,CR<Int> width,CR<Int> height,CR<Color> bgcolor,CR<Int> samples):ray(ray),ud(ud),rd(rd),width(width),height(height),bgcolor(bgcolor),samples(samples){}
        Color render(CR<Ray> ray,CR<Int> rtd)const{
            Ptr<const Renderable::Point> point(nullptr);
            for(const auto &fp:*this)
                if(const auto t=fp->pick(ray);t.get()!=nullptr&&(point.get()==nullptr||t->dist<point->dist)) point=t;
            if(point.get()==nullptr) return {};
            const auto [cpoint,normal,color,light,dist,roughn]=*point.get();
            //if(rtd) return render({cpoint,((ray.ray-normal*(ray.ray*normal)*2).unit()*2.0+Vector::randomUnit()*roughn).unit()},rtd-1).scale(color)+light;
            if(rtd) return render({cpoint,((ray.ray-normal*(ray.ray*normal)*2).unit()*2.0+Vector::randomUnit()*roughn).unit()},rtd-1).scale(color)*((-ray.ray)*normal)+light;
            return light;
        }
        Color render(CR<Int> x,CR<Int> y,CR<Int> rtd)const{
            const Int hh=height>>1,hw=width>>1;
            Color res;
            for(Int i=0;i<samples;++i) res+=render({ray.pos,(ray.ray+ud*(hh-y+dist(engine))+rd*(x-hw+dist(engine))).unit()},rtd);
            return res/samples;
        }
    };
}