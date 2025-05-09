#pragma once
#include<c3d/c3d.h>
#include<cmath>
namespace c3d{
    struct vector{
        float x,y,z;
        void rotate(const vector &origin,const vector &axis,float a);
        void rotate(const vector &axis,float a);
        void unitize();
    };
    inline vector operator+(const vector &a,const vector &b){
        return{a.x+b.x,a.y+b.y,a.z+b.z};
    }
    inline vector &operator+=(vector &a,const vector &b){
        return a.x+=b.x,a.y+=b.y,a.z+=b.z,a;
    }
    inline vector operator-(const vector &a,const vector &b){
        return{a.x-b.x,a.y-b.y,a.z-b.z};
    }
    inline vector &operator-=(vector &a,const vector &b){
        return a.x-=b.x,a.y-=b.y,a.z-=b.z,a;
    }
    inline vector operator*(const vector &v,const float a){
        return{v.x*a,v.y*a,v.z*a};
    }
    inline float operator*(const vector &a,const vector &b){
        return a.x*b.x+a.y*b.y+a.z*b.z;
    }
    inline vector operator&(const vector &a,const vector &b){
        return{a.y*b.z-a.z*b.y,a.z*b.x-a.x*b.z,a.x*b.y-a.y*b.x};
    }
    inline vector operator/(const vector &v,const float a){
        return{v.x/a,v.y/a,v.z/a};
    }
    inline vector &operator/=(vector &v,const float a){
        return v.x/=a,v.y/=a,v.z/=a,v;
    }
    inline vector operator-(const vector &v){
        return{-v.x,-v.y,-v.z};
    }
    inline float self_dot(const vector &v){
        return v.x*v.x+v.y*v.y+v.z*v.z;
    }
    inline float norm(const vector &v){
        return std::sqrt(self_dot(v));
    }
    inline vector unit(const vector &v){
        return v/norm(v);
    }
    inline vector rotate(const vector &v,const vector &axis,const float a){
        const float c=std::cos(a);return v*c+axis*(1-c)*(v*axis)+(v&axis)*std::sin(a);
    }
    inline vector rotate(const vector &v,const vector &origin,const vector &axis,const float a){
        return rotate(v-origin,axis,a)+origin;
    }
    inline bool operator==(const vector &a,const vector &b){
        return a.x==b.x&&a.y==b.y&&a.z==b.z;
    }
    inline void vector::rotate(const vector &origin,const vector &axis,const float a){
        *this-=origin;
        rotate(axis,a);
        *this+=origin;
    }
    inline void vector::rotate(const vector &axis,const float a){
        const float c=std::cos(a);
        *this=*this*c+axis*(1-c)*(*this*axis)+(*this&axis)*std::sin(a);
    }
    inline void vector::unitize(){
        *this/=norm(*this);
    }
    inline vector random_unit_vector(){
        const float b=random_unit_float(),r=std::sqrt(b*(b-1))*2,l=2*PI*random_unit_float();
        return{std::cos(l)*r,std::sin(l)*r,1-2*b};
    }
    inline vector random_unit_vector_on_hemisphere(const vector &n){
        const float b=random_unit_float(),r=std::sqrt(b*(b-1))*2,l=2*PI*random_unit_float();
        const vector v{std::cos(l)*r,std::sin(l)*r,1-2*b};
        return v*n>0?v:-v;
    }
    inline vector cosine_random_unit_vector(){
        const float a=2*PI*random_float(),b=random_float(),c=std::sqrt(b);
        return{std::cos(a)*c,std::sin(a)*c,std::sqrt(1-b)};
    }
}