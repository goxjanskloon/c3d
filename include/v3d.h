#pragma once
#define _USE_MATH_DEFINES
#include<cmath>
#include<random>
namespace v3d{
    struct vec3{double x,y,z;};
    vec3 operator+(const vec3 &a,const vec3 &b){return{a.x+b.x,a.y+b.y,a.z+b.z};}
    vec3 operator-(const vec3 &a,const vec3 &b){return{a.x-b.x,a.y-b.y,a.z-b.z};}
    vec3 operator*(const vec3 &v,const double &a){return{v.x*a,v.y*a,v.z*a};}
    double operator*(const vec3 &a,const vec3 &b){return a.x*b.x+a.y*b.y+a.z*b.z;}
    vec3 operator&(const vec3 &a,const vec3 &b){return{a.y*b.z-a.z*b.y,a.z*b.x-a.x*b.z,a.x*b.y-a.y*b.x};}
    vec3 operator/(const vec3 &v,const double &a){return{v.x/a,v.y/a,v.z/a};}
    vec3 operator/=(vec3 &v,const double &a){return v.x/=a,v.y/=a,v.z/=a,v;}
    vec3 operator-(const vec3 &v){return{-v.x,-v.y,-v.z};}
    double normsq(const vec3 &v){return v.x*v.x+v.y*v.y+v.z*v.z;}
    double norm(const vec3 &v){return sqrt(normsq(v));}
    vec3 unit(const vec3 &v){return v/norm(v);}
    vec3 &unitize(vec3 &v){return v/=norm(v);}
    vec3 rotate(const vec3 &v,const vec3 &o,const vec3 &x,const double &a){return rotate(v-o,x,a)+o;}
    vec3 rotate(const vec3 &v,const vec3 &x,const double &a){const double c=cos(a);return v*c+x*(1-c)*(v*x)+(v&x)*sin(a);}
    template<typename T>vec3 randuvec3(T g){
        static std::uniform_real_distribution<> d(-1,1);
        const double b=d(g),r=sqrt(b*(b-1))*2,l=2*M_PI*d(g);
        return{cos(l)*r,sin(l)*r,1-2*b};
    }
    template<typename T>vec3 randhvec3(T g,vec3 n){
        static std::uniform_real_distribution<> d(-1,1);
        const double b=d(g),r=sqrt(b*(b-1))*2,l=2*M_PI*d(g);
        return{cos(l)*r,sin(l)*r,1-2*b};
    }
    struct hitrec{
        vec3 p,n,c;
        double b;
    }
    struct object{

    }
}