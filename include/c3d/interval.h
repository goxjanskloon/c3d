#pragma once
#include<algorithm>
#include<c3d/c3d.h>
namespace c3d {
    struct interval{
        float min,max;
        [[nodiscard]] float clamp(const float a)const{
            return a<min?min:a>max?max:a;
        }
        [[nodiscard]] bool contain(const float a)const{
            return min<=a&&a<=max;
        }
        [[nodiscard]] bool empty()const{
            return min>max;
        }
        void expand(const float a){
            min-=a;
            max+=a;
        }
        interval &intersect(const interval &a){
            if(a.min>min)
                min=a.min;
            if(a.max<max)
                max=a.max;
            return *this;
        }
        void unite(const interval &a){
            if(a.min<min)
                min=a.min;
            if(a.max>max)
                max=a.max;
        }
        [[nodiscard]] float length()const{
            return max-min;
        }
    };
    constexpr interval universe_interval{-INF,INF},empty_interval{INF,-INF};
    inline interval unite(const interval &a,const interval &b){
        return{std::min(a.min,b.min),std::max(a.max,b.max)};
    }
    inline interval intersect(const interval &a,const interval &b){
        return{std::max(a.min,b.min),std::min(a.max,b.max)};
    }
}