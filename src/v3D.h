#pragma once
#include<EGE/graphics.h>
#include<future>
#include<array>
#include<cmath>
#include<list>
inline double sind(const double &a){return sin(0.017453292519943295*a);}
inline double cosd(const double &a){return cos(0.017453292519943295*a);}
class vector3d{
public:
    double x,y,z;
    vector3d operator+(const double &v)const{return {x+v,y+v,z+v};}
    vector3d &operator+=(const double &v){x+=v,y+=v,z+=v;return *this;}
    vector3d operator-(const double &v)const{return {x-v,y-v,z-v};}
    vector3d &operator-=(const double &v){x-=v,y-=v,z-=v;return *this;}
    vector3d operator+(const vector3d &v)const{return {x+v.x,y+v.y,z+v.z};}
    vector3d &operator+=(const vector3d &v){x+=v.x,y+=v.y,z+=v.z;return *this;}
    vector3d operator-(const vector3d &v)const{return {x-v.x,y-v.y,z-v.z};}
    vector3d &operator-=(const vector3d &v){x-=v.x,y-=v.y,z-=v.z;return *this;}
    vector3d operator*(const double &a)const{return {x*a,y*a,z*a};}
    vector3d &operator*=(const double &a){x*=a,y*=a,z*=a;return *this;}
    vector3d &scale(const vector3d &c,const vector3d &v){return ((*this-=c)*=v)+=c;}
    double operator*(const vector3d &v)const{return x*v.x+y*v.y+z*v.z;}
    vector3d &operator*=(const vector3d &v){x*=v.x,y*=v.y,z*=v.z;return *this;}
    vector3d operator/(const double &a)const{return {x/a,y/a,z/a};}
    vector3d &operator/=(const double &a){x/=a,y/=a,z/=a;return *this;}
    vector3d operator&(const vector3d &v)const{return {y*v.z-z*v.y,z*v.x-x*v.z,x*v.y-y*v.x};}
    vector3d &operator&=(const vector3d &v){x=y*v.z-z*v.y,y=z*v.x-x*v.z,z=x*v.y-y*v.x;return *this;}
    vector3d &rotate(const vector3d &c,const double &dx,const double &dy,const double &dz);
    vector3d protate(const vector3d &c,const double &dx,const double &dy,const double &dz)const{return vector3d(*this).rotate(c,dx,dy,dz);}
    double norm()const{return sqrt(x*x+y*y+z*z);}
    const vector3d &center()const{return *this;}
};
template<typename objT,typename ctrT=std::list<objT>>
class contnr3d:public ctrT{
public:
    contnr3d(){}
    contnr3d(const ctrT &objs_):ctrT(objs_){}
    contnr3d &operator+=(const vector3d &v){
        for(auto &p:*this) p+=v;
        return *this;
    }
    contnr3d &scale(const vector3d &c,const double &sx,const double &sy,const double &sz){
        for(auto &p:*this) p.scale(c,sx,sy,sz);
        return *this;
    }
    contnr3d &rotate(const vector3d &c,const double &dx,const double &dy,const double &dz){
        for(auto &p:*this) p.rotate(c,dx,dy,dz);
        return *this;
    }
    vector3d center()const{
        vector3d c{0,0,0};
        for(const auto &p:*this) c+=p.center();
        return c/this->size();
    }
};
class triface3d:public contnr3d<vector3d,std::array<vector3d,3>>{
public:
    color_t color=0;
    triface3d(){}
    triface3d(const vector3d &v1,const vector3d &v2,const vector3d &v3,const color_t &color_):contnr3d({v1,v2,v3}),color(color_){}
};
class renderer3d:public std::list<triface3d*>{
public:
    vector3d pos,facing,ud,rd;
    int width,height,hw,hh;
    renderer3d(){}
    renderer3d(const vector3d &pos,const vector3d &facing,const vector3d &ud,const vector3d &rd,const int &width,const int &height):pos(pos),facing(facing),ud(ud),rd(rd),width(width),height(height),hw(width>>1),hh(height>>1){}
    void render_pixel(const int &lx,const int &rx,const int &ly,const int &ry)const;
};
class rect3d:public contnr3d<triface3d,std::array<triface3d,12>>{
public:
    rect3d(){}
    rect3d(const vector3d &a,const vector3d &b,const color_t(&colors)[6]);
    std::pair<vector3d,vector3d> box()const;
};
bool is_collided(const std::pair<vector3d,vector3d> &r1,const std::pair<vector3d,vector3d> &r2);
class renderer3d_guard{
public:
    renderer3d *rd;
    std::list<std::list<triface3d*>::iterator> fp;
    bool face_removed=0;
    template<typename ctrT>
    renderer3d_guard(contnr3d<triface3d,ctrT> &ctr,renderer3d *const& rd_):rd(rd_){
        for(auto &p:ctr) rd->push_back(&p),fp.emplace_back(prev(rd->end()));
    }
    ~renderer3d_guard(){
        if(!face_removed)
            for(auto &p:fp) rd->erase(p);
    }
    bool remove(){
        if(!face_removed){
            face_removed=1;
            for(auto &p:fp) rd->erase(p);
            fp.erase(fp.begin(),fp.end());
            return 1;
        }
        return 0;
    }
};
