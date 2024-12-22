#pragma once
#include<limits>
#include<random>
namespace v3d{
constexpr double INF=std::numeric_limits<double>::infinity(),PI=std::acos(-1),EPSILON=1e-5;
using Dimension=char;
constexpr Dimension X='X',Y='Y',Z='Z';
class Interval{
public:
    double min,max;
    const double& clamp(const double& a)const{return a<min?min:a>max?max:a;}
    bool contain(const double& a)const{return min<=a&&a<=max;}
    bool valid()const{return min>max;}
    Interval& intersect(const Interval& a){return min=std::max(min,a.min),max=std::min(max,a.max),*this;}
    static const Interval universe,empty;
};
const Interval Interval::universe{-INF,INF},Interval::empty{INF,-INF};
inline Interval unite(const Interval& a,const Interval& b){return{std::min(a.min,b.min),std::max(a.max,b.max)};}
inline Interval intersect(const Interval& a,const Interval &b){return{std::max(a.min,b.min),std::min(a.max,b.max)};}
class Vector{
public:
    double x,y,z;
    Vector& rotate(const Vector& origin,const Vector& axis,const double& a);
    Vector& rotate(const Vector& axis,const double& a);
    Vector& unitize();
};
using Color=Vector;
inline Vector operator+(const Vector& a,const Vector& b){return{a.x+b.x,a.y+b.y,a.z+b.z};}
inline Vector& operator+=(Vector& a,const Vector& b){return a.x+=b.x,a.y+=b.y,a.z+=b.z,a;}
inline Vector operator-(const Vector& a,const Vector& b){return{a.x-b.x,a.y-b.y,a.z-b.z};}
inline Vector& operator-=(Vector& a,const Vector& b){return a.x-=b.x,a.y-=b.y,a.z-=b.z,a;}
inline Vector operator*(const Vector& v,const double& a){return{v.x*a,v.y*a,v.z*a};}
inline double operator*(const Vector& a,const Vector& b){return a.x*b.x+a.y*b.y+a.z*b.z;}
inline Vector operator&(const Vector& a,const Vector& b){return{a.y*b.z-a.z*b.y,a.z*b.x-a.x*b.z,a.x*b.y-a.y*b.x};}
inline Vector operator/(const Vector& v,const double& a){return{v.x/a,v.y/a,v.z/a};}
inline Vector& operator/=(Vector& v,const double& a){return v.x/=a,v.y/=a,v.z/=a,v;}
inline Vector operator-(const Vector& v){return{-v.x,-v.y,-v.z};}
inline double normSq(const Vector& v){return v.x*v.x+v.y*v.y+v.z*v.z;}
inline double norm(const Vector& v){return sqrt(normSq(v));}
inline Vector unit(const Vector& v){return v/norm(v);}
inline Vector rotate(const Vector& v,const Vector& axis,const double& a){const double c=std::cos(a);return v*c+axis*(1-c)*(v*axis)+(v&axis)*std::sin(a);}
inline Vector rotate(const Vector& v,const Vector& origin,const Vector& axis,const double& a){return rotate(v-origin,axis,a)+origin;}
inline bool operator==(const Vector& a,const Vector& b){return a.x==b.x&&a.y==b.y&&a.z==b.z;}
inline Vector& Vector::rotate(const Vector& origin,const Vector&axis,const double&a){return *this-=origin,*this=rotate(axis,a)+origin;}
inline Vector& Vector::rotate(const Vector& axis,const double&a){const double c=std::cos(a);return *this=*this*c+axis*(1-c)*(*this*axis)+(*this&axis)*std::sin(a);}
inline Vector& Vector::unitize(){return *this/=norm(*this);}
template<typename G>Vector RandUnitVec3(G& generator){
    static std::uniform_real_distribution<> d(-1,1);
    const double b=d(generator),r=sqrt(b*(b-1))*2,l=2*PI*d(generator);
    return{std::cos(l)*r,std::sin(l)*r,1-2*b};
}
template<typename G>Vector RandVec3OnUnitHemisphere(G& generator,const Vector& n){
    static const std::uniform_real_distribution<> d(-1,1);
    const double b=d(generator),r=sqrt(b*(b-1))*2,l=2*PI*d(generator);
    const Vector v{std::cos(l)*r,std::sin(l)*r,1-2*b};
    return v*n>0?v:-v;
}
class Material{
public:
    virtual ~Material()=0;
    virtual double possibility(const Vector& theoretic,const Vector& real)const=0;
    virtual Vector generate(const Vector& normal,const Vector& theoretic)const=0;
};
class Aabb{
public:
    Interval x,y,z;
    Aabb(const Interval& x,const Interval& y,const Interval& z):x(x),y(y),z(z){}
    Aabb(const Vector& a,const Vector& b):x{std::min(a.x,b.x),std::max(a.x,b.x)},y{std::min(a.y,b.y),std::max(a.y,b.y)},z{std::min(a.z,b.z),std::max(a.z,b.z)}{}
    Aabb(const Aabb& a,const Aabb& b):x(v3d::unite(a.x,b.x)),y(v3d::unite(a.y,b.y)),z(v3d::unite(a.z,b.z)){}
    bool hit(const Vector& origin,const Vector& ray,Interval interval)const{
        return !(interval.intersect({(x.min-origin.x)/ray.x,(x.max-origin.x)/ray.x}).valid()
               ||interval.intersect({(y.min-origin.y)/ray.y,(y.max-origin.y)/ray.y}).valid()
               ||interval.intersect({(z.min-origin.z)/ray.z,(z.max-origin.z)/ray.z}).valid());
    }
    Aabb& unite(const Aabb& a){return x.intersect(a.x),y.intersect(a.y),z.intersect(a.z),*this;}
    static const Aabb empty;
};
const Aabb Aabb::empty{Interval::empty,Interval::empty,Interval::empty};
struct Light{
    Color color;
    double brightness;
};
struct HitRecord{
    Vector point,normal;
    std::shared_ptr<const Light> light;
    double dist;
    std::shared_ptr<const Material> material;
};
class Hittable{
public:
    virtual ~Hittable()=0;
    virtual std::shared_ptr<HitRecord> hit(const Vector& origin,const Vector& ray,const Interval& interval)const=0;
    virtual Aabb aabb()const=0;
};
class Mirror final:public Material{
public:
    double possibility(const Vector& theoretic,const Vector& real)const override{return real==theoretic?1:0;}
    Vector generate(const Vector& normal,const Vector& theoretic)const override{return theoretic;}
};
class BvhTree:public Hittable{
public:
    //TODO:sync with dev-java
    std::shared_ptr<const Hittable> left,right;
    Aabb aabb;
    explicit BvhTree(std::vector<std::shared_ptr<const Hittable>>& objects){
        //TODO:sync with dev-java
        const std::size_t n=objects.size();
        if(!n)
            left=right=nullptr,aabb=Aabb::empty;
        else if(n==1)
            aabb=(left=objects.front())->aabb(),right=nullptr;
        else if(n==2)
            aabb={(left=objects.front())->aabb(),(right=objects.back())->aabb()};
        else{
            Aabb tb=Aabb::empty;
            for(const auto &o:objects)
                tb.unite(o->aabb());

        }
    }
    std::shared_ptr<HitRecord> hit(const Vector& origin,const Vector& ray,const Interval&interval)const override{
        //TODO:sync with dev-java
    }
};
class Sphere:public Hittable{
public:
    Vector center;
    double radius;
    std::shared_ptr<Light> light;
    std::shared_ptr<Material> material;
    std::shared_ptr<HitRecord> hit(const Vector& origin,const Vector& ray,const Interval&interval)const override{
        //TODO:sync with dev-java
        const Vector co=origin-center;
        const double b=ray*co,d=b*b-normSq(co)+radius*radius;
        if(d<0)
            return nullptr;
        const double sd=std::sqrt(d);
        double t=-b-sd;
        if(t<EPSILON)
            t+=sd*2;
        if(t<EPSILON)
            return nullptr;
        const Vector point=origin+ray*t;
        return std::make_shared<HitRecord>(HitRecord{point,(point-center).unitize(),light,t,material});
    }
    Aabb aabb()const override{return{{center.x-radius,center.x+radius},{center.y-radius,center.y+radius},{center.z-radius,center.z+radius}};}
};
}