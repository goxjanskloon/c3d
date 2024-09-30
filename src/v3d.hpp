#pragma once
#ifdef _MSC_VER
#define NOMINMAX
#endif
#include<array>
#include<cfloat>
#include<cmath>
#include<ege.h>
#include<future>
#include<list>
namespace v3d{
inline double sind(const double &a){return sin(0.017453292519943295*a);}
inline double cosd(const double &a){return cos(0.017453292519943295*a);}
class vector{
public:
    double x,y,z;
    vector operator+(const double &v)const{return {x+v,y+v,z+v};}
    vector &operator+=(const double &v){x+=v,y+=v,z+=v;return *this;}
    vector operator-(const double &v)const{return {x-v,y-v,z-v};}
    vector &operator-=(const double &v){x-=v,y-=v,z-=v;return *this;}
    vector operator+(const vector &v)const{return {x+v.x,y+v.y,z+v.z};}
    vector &operator+=(const vector &v){x+=v.x,y+=v.y,z+=v.z;return *this;}
    vector operator-(const vector &v)const{return {x-v.x,y-v.y,z-v.z};}
    vector &operator-=(const vector &v){x-=v.x,y-=v.y,z-=v.z;return *this;}
    vector operator*(const double &a)const{return {x*a,y*a,z*a};}
    vector &operator*=(const double &a){x*=a,y*=a,z*=a;return *this;}
    vector &scale(const vector &c,const vector &v){return ((*this-=c)*=v)+=c;}
    double operator*(const vector &v)const{return x*v.x+y*v.y+z*v.z;}
    vector &operator*=(const vector &v){x*=v.x,y*=v.y,z*=v.z;return *this;}
    vector operator/(const double &a)const{return {x/a,y/a,z/a};}
    vector &operator/=(const double &a){x/=a,y/=a,z/=a;return *this;}
    vector operator&(const vector &v)const{return {y*v.z-z*v.y,z*v.x-x*v.z,x*v.y-y*v.x};}
    vector &operator&=(const vector &v){x=y*v.z-z*v.y,y=z*v.x-x*v.z,z=x*v.y-y*v.x;return *this;}
    vector &rotate(const vector &c,const double &dx,const double &dy,const double &dz);
    [[nodiscard]] vector protate(const vector &c,const double &dx,const double &dy,const double &dz)const{return vector(*this).rotate(c,dx,dy,dz);}
    [[nodiscard]] double norm()const{return sqrt(x*x+y*y+z*z);}
    [[nodiscard]] const vector &center()const{return *this;}
};
template<typename objT,typename ctrT=std::list<objT>>
class container:public ctrT{
public:
    container()=default;
    explicit container(const ctrT &objs_):ctrT(objs_){}
    container &operator+=(const vector &v){
        for(auto &p:*this) p+=v;
        return *this;
    }
    container &scale(const vector &c,const double &sx,const double &sy,const double &sz){
        for(auto &p:*this) p.scale(c,sx,sy,sz);
        return *this;
    }
    container &rotate(const vector &c,const double &dx,const double &dy,const double &dz){
        for(auto &p:*this) p.rotate(c,dx,dy,dz);
        return *this;
    }
    [[nodiscard]] vector center()const{
        vector c{0,0,0};
        for(const auto &p:*this) c+=p.center();
        return c/static_cast<double>(this->size());
    }
};
class renderable{
public:
    virtual ~renderable()=default;
    [[nodiscard]] virtual std::pair<double,ege::color_t> pick(const vector &pos,const vector &ray,const int &rtd)const{return {-DBL_MAX,0};}
};
class renderer:public std::list<const renderable*>{
public:
    vector pos,facing,ud,rd;
    int width,height,hw,hh;
    renderer():pos(),facing(),ud(),rd(),width(),height(),hw(),hh(){}
    renderer(const vector &pos,const vector &facing,const vector &ud,const vector &rd,const int &width,const int &height):pos(pos),facing(facing),ud(ud),rd(rd),width(width),height(height),hw(width>>1),hh(height>>1){}
    [[nodiscard]] ege::color_t render(const int &x,const int &y,const int &rtd)const;
};
class triface:public container<vector,std::array<vector,3>>,public renderable{
public:
    ege::color_t color;
    triface():color(){}
    triface(const vector &v1,const vector &v2,const vector &v3,const ege::color_t &color_):container({v1,v2,v3}),color(color_){}
    [[nodiscard]] std::pair<double,ege::color_t> pick(const vector&pos,const vector&ray,const int&rtd)const override;
};
class rect:public container<triface,std::array<triface,12>>{
public:
    rect()=default;
    rect(const vector &a,const vector &b,const ege::color_t(&colors)[6]);
    [[nodiscard]] std::pair<vector,vector> box()const;
};
bool is_collided(const std::pair<vector,vector> &r1,const std::pair<vector,vector> &r2);
class renderer_guard{
public:
    renderer *rd;
    std::list<std::list<const renderable*>::iterator> fp;
    bool face_removed=false;
    template<typename objT,typename ctrT,typename = std::enable_if_t<std::is_base_of_v<renderable,objT>>>
    renderer_guard(const container<objT,ctrT> &ctr,renderer *const& rd_):rd(rd_){
        for(const auto &p:ctr) rd->emplace_back(&p),fp.emplace_back(prev(rd->end()));
    }
    ~renderer_guard(){
        if(!face_removed)
            for(const auto &p:fp) rd->erase(p);
    }
    bool remove(){
        if(!face_removed){
            face_removed=true;
            for(const auto &p:fp) rd->erase(p);
            fp.erase(fp.begin(),fp.end());
            return true;
        }
        return false;
    }
};
}