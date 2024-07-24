/**
 * @file v3D.h
 * @brief Definition of the 3D classes.
 * @author goxjansloon
 * @date 2024-7-24
 * @version v0.1.0-alpha
 * @copyright Copyright (C) 2024  goxjanskloon
 */
#pragma once
#ifdef _MSC_VER
#define NOMINMAX //To avoid conflicts between std::min/max and MSVC's min/max macros.
#endif
#include<EGE/ege.h>
#include<future>
#include<array>
#include<cmath>
#include<list>

///These trigonometric functions are for angles indegree,not in radian.The strange number below approximately equals to Pi/180
inline double sind(const double &a){return sin(0.017453292519943295*a);}
inline double cosd(const double &a){return cos(0.017453292519943295*a);}

/**
 * @brief 3D Vector
 */
class vector3d{
public:
    double x,y,z;

    //Basic operations
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
    
    /**
     * @brief Scale the vector with a center.
     * @param c The center.
     * @param v The scaling factor.
     * @return Self.
     */
    vector3d &scale(const vector3d &c,const vector3d &v){return ((*this-=c)*=v)+=c;}
    
    ///@brief Dot product.
    double operator*(const vector3d &v)const{return x*v.x+y*v.y+z*v.z;}
    
    ///@brief Multiply each dimension of 'this' ans 'v'.
    ///@warning This function is different from 'operator*()'!
    vector3d &operator*=(const vector3d &v){x*=v.x,y*=v.y,z*=v.z;return *this;}
    
    ///Divide each dimension of 'this' by 'v'.
    vector3d operator/(const double &a)const{return {x/a,y/a,z/a};}
    vector3d &operator/=(const double &a){x/=a,y/=a,z/=a;return *this;}

    ///@brief Cross product.
    vector3d operator&(const vector3d &v)const{return {y*v.z-z*v.y,z*v.x-x*v.z,x*v.y-y*v.x};}
    vector3d &operator&=(const vector3d &v){x=y*v.z-z*v.y,y=z*v.x-x*v.z,z=x*v.y-y*v.x;return *this;}
    
    /**
     * @brief Rotate the vector with a center.
     * @param c The rotating center.
     * @param dx,dy,dz The angle to rotate in three dimensions, in degree.
     * @return Self.
    */
    vector3d &rotate(const vector3d &c,const double &dx,const double &dy,const double &dz);
    
    ///@brief Similar to 'rotate' but produces, not modifies.
    vector3d protate(const vector3d &c,const double &dx,const double &dy,const double &dz)const{return vector3d(*this).rotate(c,dx,dy,dz);}
    
    ///@brief Norm of the vector.
    double norm()const{return sqrt(x*x+y*y+z*z);}

    /**
     * @brief This function is meaningless but supports unite interface for 'contnr3d'.
     * @see contnr3d::center()
     */
    const vector3d &center()const{return *this;}
};

/**
 * @brief Universal container of 3D objects.
 * The container passes several common operations to the objects it contains.
 * @param objT The type of objects to be contained.
 * @param ctrT The base container to contain objects.The default container is 'std::list<objT>'
 */
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

    ///@brief Calculate the 'center' of the container, that is the average of the centers of the objects;
    vector3d center()const{
        vector3d c{0,0,0};
        for(const auto &p:*this) c+=p.center();
        return c/static_cast<double>(this->size());
    }
};

///@brief 3D trangle face.
class triface3d:public contnr3d<vector3d,std::array<vector3d,3>>{
public:
    ege::color_t color=0;///The color of the face.
    triface3d(){}
    ///@param v1,v2,v3 Three vertices of the face
    triface3d(const vector3d &v1,const vector3d &v2,const vector3d &v3,const ege::color_t &color_):contnr3d({v1,v2,v3}),color(color_){}
};

///@brief renderer for 3D triangle faces.
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
    rect3d(const vector3d &a,const vector3d &b,const ege::color_t(&colors)[6]);
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
