#pragma once
#include<execution>
#include<c3d/aabb.h>
#include<c3d/hittable.h>
#include<c3d/vector.h>
namespace c3d{
    class bvh_tree final:public hittable{
        bvh_tree(std::vector<std::shared_ptr<const hittable>> &objects,const int l,const int r):aabb_(){
            aabb_=empty_aabb;
            if(const std::size_t n=objects.size();n==1) {
                aabb_=(left=objects.front())->bounding_box();
                right=nullptr;
            } else if(n==2)
                aabb_={(left=objects.front())->get_aabb(),(right=objects.back())->get_aabb()};
            else {
                aabb_=objects.front()->get_aabb();
                for(auto p=std::next(objects.begin());p!=objects.end();++p)
                    aabb_.unite((*p)->get_aabb());
                const float xl=aabb_.x.length(),yl=aabb_.y.length(),zl=aabb_.z.length();
                const auto lp=objects.begin()+l,rp=objects.begin()+r;
                std::sort(std::execution::par_unseq,lp,rp,
                          [&](const std::shared_ptr<const hittable>&a,const std::shared_ptr<const hittable>&b) {
                              if(xl>yl&&xl>zl)
                                  return a->get_aabb().x.min<b->get_aabb().x.min;
                              if(yl>xl&&yl>zl)
                                  return a->get_aabb().y.min<b->get_aabb().y.min;
                              return a->get_aabb().z.min<b->get_aabb().z.min;
                          });
                const int m=l+r>>1;
                left=std::shared_ptr<hittable>(new bvh_tree(objects,l,m));
                right=std::shared_ptr<hittable>(new bvh_tree(objects,m+1,r));
            }
        }
    public:
        std::shared_ptr<const hittable> left,right;
        aabb aabb_;
        explicit bvh_tree(std::vector<std::shared_ptr<const hittable>> &objects):bvh_tree(objects,0,static_cast<int>(objects.size())-1){}
        [[nodiscard]] std::shared_ptr<hit_record> hit(const ray &ray_,const interval &interval_)const override{
            auto left_hit=left==nullptr?nullptr:left->hit(ray_,interval_),right_hit=right==nullptr?nullptr:right->hit(ray_,interval_);
            if(left_hit==nullptr) return right_hit;
            if(right_hit==nullptr) return left_hit;
            return left_hit->distance<right_hit->distance?left_hit:right_hit;
        }
        [[nodiscard]] aabb get_aabb()const override{
            return aabb_;
        }
    };
}