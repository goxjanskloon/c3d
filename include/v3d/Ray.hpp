#pragma once
#include<v3d/definitions.hpp>
#include<v3d/Vector.hpp>
namespace v3d{
    class Ray{
    public:
        Vector pos,ray;
        Vector at(CR<Real> t)const{return pos+ray*t;}
    };
    inline Vector &Vector::rotate(CR<Ray> axis,CR<Real> angle){
        *this-=axis.ray;
        const Real cosa=cos(angle);
        return *this=*this*cosa+axis.ray*(1-cosa)*(axis.ray**this)+(axis.ray*sin(angle)&*this)+axis.pos;
    }
}