#pragma once
#include<c3d/vector.h>
namespace c3d{
    struct onb{
        vector u,v,w;
        explicit onb(const vector &n):w(n){
            u=w&(v=unit(w&(std::abs(w.x)>.9f?vector{0.f,1.f,0.f}:vector(1.f,0.f,0.f))));
        }
        [[nodiscard]] vector transform(const vector &p)const{
            return u*p.x+v*p.y+w*p.z;
        }
    };
}