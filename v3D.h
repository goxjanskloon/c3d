#pragma once
#include"EGE/graphics.h"
#include<utility>
#include<cstddef>
#include<array>
#include<cmath>
#include<list>
#define SHOW_CONSOLE 0
inline double sind(const double &a){ return sin(0.017453292519943295*a); }
inline double cosd(const double &a){ return cos(0.017453292519943295*a); }
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
    vector3d operator/(const double &a)const{return {x/a,y/a,z/a};}
    vector3d &operator/=(const double &a){x/=a,y/=a,z/=a;return *this;}
    double operator*(const vector3d &v)const{return x*v.x+y*v.y+z*v.z;}
    vector3d operator&(const vector3d &v)const{return {y*v.z-v.y*z,v.x*z-z*v.z,x*v.y-v.x*y};}
    vector3d &operator&=(const vector3d &v){x=y*v.z-v.y*z,y=v.x*z-z*v.z,z=x*v.y-v.x*y;return *this;}
    vector3d &vector3d::rotate(const vector3d &c,const double &dx,const double &dy,const double &dz);
    double norm()const{return sqrt(x+y+z);}
};
template<typename objT,typename ctrT=std::list<objT>>
class contnr3d:public ctrT{
public:
    contnr3d(){}
    contnr3d(const ctrT &objs_):ctrT(objs_){}
    contnr3d &move(const double &tx,const double &ty,const double &tz){
        for(auto &p:*this) p.move(tx,ty,tz);
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
class face3d:public contnr3d<vector3d>{
public:
    color_t color=0;
    face3d(){}
    face3d(const std::list<vector3d> &faces,const color_t &color_):contnr3d(faces),color(color_){}
};
class render3d:public std::list<face3d*>{
public:
    void render(const vector3d &pos,const vector3d &facing,const vector3d &dir,const int &width,const int &height,const PIMAGE &pimg);
};
class rect3d:public contnr3d<face3d,std::array<face3d,6>>{
public:
    rect3d(){}
    rect3d(const vector3d &a,const vector3d &b,const color_t(&colors)[6]);
    std::pair<vector3d,vector3d> collisionbox()const;
};
class render3d_guard{
public:
    render3d *cmr;
    std::list<std::list<face3d*>::iterator> fp;
    bool face_removed=0;
    template<typename ctrT>
    render3d_guard(const contnr3d<face3d,ctrT> &ctr,render3d *const& cmr_):cmr(cmr){
        for(auto &p:ctr) cmr->push_back(&ctr[i]),fp.emplace_back(prev(rd->end()));
    }
    ~render3d_guard(){
        if(!face_removed)
            for(auto &p:fp) cmr->erase(p);
    }
    bool remove(){
        if(!face_removed){
            face_removed=1;
            for(auto &p:fp) cmr->erase(p);
            fp.erase(fp.begin(),fp.end());
            return 1;
        }
        return 0;
    }
};
byte colliding(const std::pair<vector3d,vector3d> &r1,const std::pair<vector3d,vector3d> &r2);
