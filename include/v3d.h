#pragma once
#define _USE_MATH_DEFINES
#include<cmath>
#include<random>
namespace v3d{
    class Interval{
    public:
        double min,max;
        const double &clamp(const double &a)const{return a<min?min:a>max?max:a;}
        bool contain(const double &a)const{return min<=a&&a<=max;}
        bool empty()const{return min>max;}
    };
    static const Interval kUniverseInterval{-DBL_MAX,DBL_MAX},kEmptyInterval{DBL_MAX,-DBL_MAX};
    Interval unite(const Interval &a,const Interval &b){return{std::min(a.min,b.min),std::max(a.max,b.max)};}
    class Vector{
    public:
        double x,y,z;
        Vector &rotate(const Vector &origin,const Vector &axis,const double &a){return *this-=origin,*this=rotate(*this,axis,a)+origin;}
        Vector &rotate(const Vector &axis,const double &a){const double c=cos(a);return *this=*this*c+axis*(1-c)*(*this*axis)+(*this&axis)*sin(a);}
        Vector &unitize(){return *this/=norm(*this);}
    };
    using Color=Vector;
    Vector operator+(const Vector &a,const Vector &b){return{a.x+b.x,a.y+b.y,a.z+b.z};}
    Vector operator+=(Vector &a,const Vector &b){return a.x+=b.x,a.y+=b.y,a.z+=b.z,a;}
    Vector operator-(const Vector &a,const Vector &b){return{a.x-b.x,a.y-b.y,a.z-b.z};}
    Vector operator-=(Vector &a,const Vector &b){return a.x-=b.x,a.y-=b.y,a.z-=b.z,a;}
    Vector operator*(const Vector &v,const double &a){return{v.x*a,v.y*a,v.z*a};}
    double operator*(const Vector &a,const Vector &b){return a.x*b.x+a.y*b.y+a.z*b.z;}
    Vector operator&(const Vector &a,const Vector &b){return{a.y*b.z-a.z*b.y,a.z*b.x-a.x*b.z,a.x*b.y-a.y*b.x};}
    Vector operator/(const Vector &v,const double &a){return{v.x/a,v.y/a,v.z/a};}
    Vector operator/=(Vector &v,const double &a){return v.x/=a,v.y/=a,v.z/=a,v;}
    Vector operator-(const Vector &v){return{-v.x,-v.y,-v.z};}
    double normsq(const Vector &v){return v.x*v.x+v.y*v.y+v.z*v.z;}
    double norm(const Vector &v){return sqrt(normsq(v));}
    Vector unit(const Vector &v){return v/norm(v);}
    Vector rotate(const Vector &v,const Vector &origin,const Vector &axis,const double &a){return rotate(v-origin,axis,a)+origin;}
    Vector rotate(const Vector &v,const Vector &axis,const double &a){const double c=cos(a);return v*c+axis*(1-c)*(v*axis)+(v&axis)*sin(a);}
    bool operator==(const Vector &a,const Vector &b){return a.x==b.x&&a.y==b.y&&a.z==b.z;}
    template<typename G>Vector RandUnitVec3(G &generator){
        static std::uniform_real_distribution<> d(-1,1);
        const double b=d(g),r=sqrt(b*(b-1))*2,l=2*M_PI*d(g);
        return{cos(l)*r,sin(l)*r,1-2*b};
    }
    template<typename G>Vector RandVec3OnUnitHemisphere(G &generator,const Vector &n){
        static const std::uniform_real_distribution<> d(-1,1);
        const double b=d(g),r=sqrt(b*(b-1))*2,l=2*M_PI*d(g);
        Vector v(cos(l)*r,sin(l)*r,1-2*b);
        return v*n>0?v:-v;
    }
    struct HitRecord{
        Vector point,normal;
        Color color;
        double dist,bright;
        std::shared_ptr<Material> material;
    };
    class Hittable{
    public:
        virtual ~Hittable()=0;
        virtual HitRecord hit(const Vector &origin,const Vector &ray)const=0;
        virtual Aabb aabb()const=0;
    };
    class Material{
    public:
        virtual ~Material()=0;
        virtual double possibility(const Vector &theoretic,const Vector &real)const=0;
        virtual Vector generate(const Vector &normal,const Vector &theoretic)const=0;
    };
    class Mirror:public Material{
    public:
        double possibility(const Vector &theoretic,const Vector &real)const override{return real==theoretic?1:0;}
        Vector generate(const Vector &normal,const Vector &theoretic)const override{return theoretic;}
    };
    class Aabb{
    public:
        Interval x,y,z;
        Aabb(const Interval &x,const Interval &y,const Interval &z):x(x),y(y),z(z){}
        Aabb(const Vector &a,const Vector &b):x{std::min(a.x,b.x),std::max(a.x,b.x)},y{std::min(a.y,b.y),std::max(a.y,b.y)},z{std::min(a.z,b.z),std::max(a.z,b.z)}{}
        Aabb(const Aabb &a,const Aabb &b):x(unite(a.x,b.x)),y(unite(a.y,b.y)),z(unite(a.z,b.z)){}
        bool hit(const Vector &origin,const Vector &ray)const{
            //TODO:sync with dev-java
        }
    };
    class Sphere:public Hittable{
    public:
        Vector center;
        double radius;
        HitRecord hit(const Vector &origin,const Vector &ray)const override{
            //TODO:sync with dev-java
        }
        Aabb aabb()const override{return{{center.x-radius,center.x+radius},{center.y-radius,center.y+radius},{center.z-radius,center.z+radius}};}
    };
}