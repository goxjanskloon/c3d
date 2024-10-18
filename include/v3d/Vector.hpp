#pragma once
#include<cmath>
#include <random>
#include<v3d/definitions.hpp>
namespace v3d{
    class Ray;
    class Vector{
    public:
        Real x,y,z;
        constexpr Vector():x(0.0),y(0.0),z(0.0){};
        constexpr Vector(CR<Real> x,CR<Real> y,CR<Real> z):x(x),y(y),z(z){}
        Vector operator+(CR<Vector> v)const{return {x+v.x,y+v.y,z+v.z};}
        Vector operator+(CR<Real> a)const{return {x+a,y+a,z+a};}
        Vector &operator+=(CR<Vector> v){x+=v.x,y+=v.y,z+=v.z;return *this;}
        Vector operator-(CR<Vector> v)const{return {x-v.x,y-v.y,z-v.z};}
        Vector operator-(CR<Real> a)const{return {x-a,y-a,z-a};}
        Vector &operator-=(CR<Vector> v){x-=v.x,y-=v.y,z-=v.z;return *this;}
        Vector operator*(CR<Real> a)const{return {x*a,y*a,z*a};}
        Vector &operator*=(CR<Real> a){x*=a,y*=a,z*=a;return *this;}
        Vector scale(CR<Vector> v)const{return {x*v.x,y*v.y,z*v.z};}
        Real operator*(CR<Vector> v)const{return x*v.x+y*v.y+z*v.z;}
        Vector operator/(CR<Real> a)const{return {x/a,y/a,z/a};}
        Vector &operator/=(CR<Real> a){x/=a,y/=a,z/=a;return *this;}
        Vector operator/(CR<Vector> v)const{return {x/v.x,y/v.y,z/v.z};}
        Vector operator&(CR<Vector> v)const{return {y*v.z-z*v.y,z*v.x-x*v.z,x*v.y-y*v.x};}
        Vector operator-()const{return {-x,-y,-z};}
        Vector &rotate(CR<Ray> axis,CR<Real> angle);
        Real norm()const{return std::sqrt(x*x+y*y+z*z);}
        Vector unit()const{return operator/(norm());}
        Vector &unitize(){return operator/=(norm());}
        static Vector randomUnit(){
            static RandomEngine engine{RandomDevice{}()};
            static std::uniform_real_distribution<double> dist{0.0,1.0};
            return Vector{dist(engine)-0.5,dist(engine)-0.5,dist(engine)-0.5}.unit();
        }
    };
}