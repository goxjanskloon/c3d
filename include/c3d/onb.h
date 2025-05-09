#pragma once
#include<c3d/vector.h>
namespace c3d{
    struct onb{
        vector u,v,w;
        explicit onb(const vector &n):w(n){
            u=w&(v=unit(w&(std::abs(w.x)>.9f?vector{.0f,1.f,.0f}:vector(1.f,.0f,.0f))));
        }
        [[nodiscard]] vector transform(const vector &p)const{
            return u*p.x+v*p.y+w*p.z;
        }
    };
}