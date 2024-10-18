#pragma once
#include<algorithm>
#include<v3d/definitions.hpp>
#include<v3d/Aabb.hpp>
#include<v3d/Renderable.hpp>
namespace v3d{
    class BvhTree:public Renderable{
    public:
        Aabb bbox;
        Ptr<const Renderable> l,r;
        template<typename T> BvhTree(T &objs):BvhTree(objs.begin(),objs.end()){}
        template<typename IFT,typename ILT> BvhTree(CR<IFT> itf,CR<ILT> itl){
            switch(itl-itf){
            case 0:l=r=nullptr,bbox=Aabb::empty;break;
            case 1:l=*itf,r=nullptr,bbox=l->getAabb();break;
            case 2:l=*itf,r=*itl,bbox={l->getAabb(),r->getAabb()};break;
            default:
                for(auto p=itf;p!=itl;++p) bbox={bbox,(*p)->getAabb()};
                const char la=bbox.getLongestAxis();
                std::sort(itf,itl,la=='X'?Aabb::compareX:la=='Y'?Aabb::compareY:Aabb::compareZ);
                const auto m=itf+((itl-itf)>>1);
                l=makePtr<BvhTree>(itf,m),r=makePtr<BvhTree>(m,itl);
            break;
            }
        }
        Aabb getAabb()const override{return bbox;}
        Ptr<Point> pick(CR<Ray> ray)const override{
            if(r==nullptr) return l==nullptr?nullptr:l->pick(ray);
            const auto lp=l->pick(ray),rp=r->pick(ray);
            return lp==nullptr?rp==nullptr?nullptr:rp:rp==nullptr?lp:lp->dist<rp->dist?lp:rp;
        }
    };
}