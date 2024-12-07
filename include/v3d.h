#pragma once
#define _USE_MATH_DEFINES
#include<cmath>
#include<random>
namespace v3d{
    
    class Vector3D{
    public:
        double x,y,z;
        Vector3D &rotate(const Vector3D &o,const Vector3D &x,const double &a){return *this-=o,*this=rotate(*this,x,a)+o;}
        Vector3D &rotate(const Vector3D &x,const double &a){const double c=cos(a);return *this=*this*c+x*(1-c)*(*this*x)+(*this&x)*sin(a);}
        Vector3D &unitize(){return *this/=norm(*this);}
    };
    Vector3D operator+(const Vector3D &a,const Vector3D &b){return{a.x+b.x,a.y+b.y,a.z+b.z};}
    Vector3D operator+=(Vector3D &a,const Vector3D &b){return a.x+=b.x,a.y+=b.y,a.z+=b.z,a;}
    Vector3D operator-(const Vector3D &a,const Vector3D &b){return{a.x-b.x,a.y-b.y,a.z-b.z};}
    Vector3D operator-=(Vector3D &a,const Vector3D &b){return a.x-=b.x,a.y-=b.y,a.z-=b.z,a;}
    Vector3D operator*(const Vector3D &v,const double &a){return{v.x*a,v.y*a,v.z*a};}
    double operator*(const Vector3D &a,const Vector3D &b){return a.x*b.x+a.y*b.y+a.z*b.z;}
    Vector3D operator&(const Vector3D &a,const Vector3D &b){return{a.y*b.z-a.z*b.y,a.z*b.x-a.x*b.z,a.x*b.y-a.y*b.x};}
    Vector3D operator/(const Vector3D &v,const double &a){return{v.x/a,v.y/a,v.z/a};}
    Vector3D operator/=(Vector3D &v,const double &a){return v.x/=a,v.y/=a,v.z/=a,v;}
    Vector3D operator-(const Vector3D &v){return{-v.x,-v.y,-v.z};}
    double normsq(const Vector3D &v){return v.x*v.x+v.y*v.y+v.z*v.z;}
    double norm(const Vector3D &v){return sqrt(normsq(v));}
    Vector3D unit(const Vector3D &v){return v/norm(v);}
    Vector3D rotate(const Vector3D &v,const Vector3D &o,const Vector3D &x,const double &a){return rotate(v-o,x,a)+o;}
    Vector3D rotate(const Vector3D &v,const Vector3D &x,const double &a){const double c=cos(a);return v*c+x*(1-c)*(v*x)+(v&x)*sin(a);}
    template<typename G>Vector3D RandUnitVec3(G &generator){
        static std::uniform_real_distribution<> d(-1,1);
        const double b=d(g),r=sqrt(b*(b-1))*2,l=2*M_PI*d(g);
        return{cos(l)*r,sin(l)*r,1-2*b};
    }
    template<typename G>Vector3D RandVec3OnUnitHemisphere(G &generator,const Vector3D &n){
        static const std::uniform_real_distribution<> d(-1,1);
        const double b=d(g),r=sqrt(b*(b-1))*2,l=2*M_PI*d(g);
        Vector3D v(cos(l)*r,sin(l)*r,1-2*b);
        return v*n>0?v:-v;
    }
    struct HitRecord{
        Vector3D point,normal,color;
        double dist,bright;
        std::shared_ptr<Material> material;
    };
    class Hittable{
    public:
        virtual ~Hittable()=0;
        virtual HitRecord hit(const Vector3D &origin,const Vector3D &ray)const=0;
        virtual Aabb aabb()=0;
    };
    class Material{
    public:
        virtual ~Material()=0;
        virtual double possibility(const Vector3D &theoretic,const Vector3D &real)=0;
        virtual Vector3D generate(const Vector3D &normal,const Vector3D &theoretic)=0;
    };
    struct Aabb{
    };
}