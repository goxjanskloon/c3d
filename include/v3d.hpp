#pragma once
#ifdef _MSC_VER
#define NOMINMAX
#endif
#include<algorithm>
#include<array>
#include<cfloat>
#include<cmath>
#include<list>
#include<memory>
#include<vector>
namespace v3d{
    constexpr double EPSILON=1e-5;
    class vector{
    public:
        double x,y,z;
        constexpr vector():x(0.0),y(0.0),z(0.0){};
        constexpr vector(const double &x,const double &y,const double &z):x(x),y(y),z(z){}
        vector operator+(const vector &v)const{return {x+v.x,y+v.y,z+v.z};}
        vector &operator+=(const vector &v){x+=v.x,y+=v.y,z+=v.z;return *this;}
        vector operator-(const vector &v)const{return {x-v.x,y-v.y,z-v.z};}
        vector &operator-=(const vector &v){x-=v.x,y-=v.y,z-=v.z;return *this;}
        vector operator*(const double &a)const{return {x*a,y*a,z*a};}
        vector &operator*=(const double &a){x*=a,y*=a,z*=a;return *this;}
        double operator*(const vector &v)const{return x*v.x+y*v.y+z*v.z;}
        vector operator/(const double &a)const{return {x/a,y/a,z/a};}
        vector &operator/=(const double &a){x/=a,y/=a,z/=a;return *this;}
        vector operator&(const vector &v)const{return {y*v.z-z*v.y,z*v.x-x*v.z,x*v.y-y*v.x};}
        vector &rotate(const vector &base,const vector &axis,const double &angle){
            *this-=base;
            const double cosa=cos(angle);
            return *this=*this*cosa+axis*(1-cosa)*(axis**this)+(axis*sin(angle)&*this)+base;
        }
        double norm()const{return sqrt(x*x+y*y+z*z);}
        vector unit()const{return operator/(norm());}
        vector &unitize(){return operator/=(norm());}
    };
    using color_t=vector;
    struct light_t{
        color_t color;
        double brightn;
        light_t():color(),brightn(0.0){};
        light_t(const color_t &color,const double &brightn):color(color),brightn(brightn){}
    };
    light_t blend(const std::initializer_list<light_t> &light){
        light_t res;
        for(auto &p:light) res.brightn+=p.brightn,res.color+=p.color*p.brightn;
        res.color/=res.brightn;
        return res;
    }
    template<typename objT,typename ctrT=std::list<objT>> class collection:public ctrT{
    public:
        collection()=default;
        explicit collection(const ctrT &objs_):ctrT(objs_){}
        collection &rotate(const vector &base,const vector &axis,const double &angle){
            for(auto &p:*this) p.rotate(base,axis,angle);
            return *this;
        }
    };
    class renderable{
    public:
        struct pickpoint_t{
            vector point,normal;
            light_t light;
            double dist,roughn;
        };
        virtual std::shared_ptr<pickpoint_t> pick(const vector &pos,const vector &ray)const=0;
    };
    class renderer:public std::list<const renderable*>{
    public:
        static constexpr unsigned int SSAA_SIZE=3;
        static constexpr unsigned int SSAA_COUNT=SSAA_SIZE*SSAA_SIZE;
        static constexpr double SSAA_OFFSET[SSAA_SIZE]{-0.33,0.0,0.33};
        vector pos,facing,ud,rd;
        int width,height;
        light_t bglight;
        renderer(const vector &pos,const vector &facing,const vector &ud,const vector &rd,const int &width,const int &height,const light_t &bglight):pos(pos),facing(facing),ud(ud),rd(rd),width(width),height(height),bglight(bglight){}
        light_t render(const vector &pos,const vector &ray,const int &rtd)const{
            using pdc=std::pair<double,color_t>;
            std::shared_ptr<renderable::pickpoint_t> point(nullptr);
            for(const auto &fp:*this)
                if(const auto t=fp->pick(pos,ray);t.get()!=nullptr&&(point.get()==nullptr||t->dist<point->dist)) point=t;
            if(point.get()==nullptr) return bglight;
            const auto &[cpoint,normal,light,dist,roughn]=*point.get();
            if(rtd) return blend({light,bglight,render(cpoint,ray-normal*(ray*normal)*2,rtd-1)/*std::min(1-roughn,(ray*-1)*normal)*/});
            return blend({light,bglight});
        }
        color_t render_ssaa(const int &x,const int &y,const int &rtd)const{
            unsigned int r=0u,g=0u,b=0u;
            const int hh=height>>1,hw=width>>1;
            for(int i=0;i<SSAA_SIZE;++i)
                for(int j=0;j<SSAA_SIZE;++j){
                    const auto c=render(pos,(facing+ud*(hh-y+SSAA_OFFSET[i])+rd*(x-hw+SSAA_OFFSET[j])).unit(),rtd);
                    r+=color_r(c),g+=color_g(c),b+=color_b(c);
                }
            return r/SSAA_COUNT<<16|g/SSAA_COUNT<<8|b/SSAA_COUNT;
        }
        color_t render(const int &x,const int &y,const int &rtd)const{
            return render(pos,(facing+ud*((height>>1)-y)+rd*(x-(width>>1))).unit(),rtd);
        }
    };
    class triface:public collection<vector,std::array<vector,3>>,public renderable{
    public:
        color_t color;
        double roughn;
        triface():color(){}
        triface(const vector &v1,const vector &v2,const vector &v3,const color_t &color,const double &roughn):collection({v1,v2,v3}),color(color),roughn(roughn){}
        virtual std::shared_ptr<pickpoint_t> pick(const vector&pos,const vector&ray)const override{
            const auto e1=at(1)-at(0),e2=at(2)-at(0),pv=ray&e2;
            double det=e1*pv;
            if(fabs(det)<EPSILON) return nullptr;
            det=1/det;
            const auto tvec=pos-at(0);
            const double u=tvec*pv*det;
            if(u<0||u>1) return nullptr;
            const auto qv=tvec&e1;
            if(const double v=ray*qv*det;v<0||u+v>1) return nullptr;
            if(const double t=e2*qv*det;t>EPSILON) return std::make_shared<pickpoint_t>(pos+ray*t,(e1&e2).unit(),color,t,roughn);
            return nullptr;
        }
    };
    class rect:public collection<triface,std::array<triface,12>>{
    public:
        template<typename arrT> rect(const vector &a,const vector &b,const arrT &colors,const double &roughn):collection({
            triface({b.x,a.y,a.z},{a.x,b.y,a.z},a,colors[0],roughn),
            triface({b.x,a.y,a.z},{a.x,b.y,a.z},{b.x,b.y,a.z},colors[0],roughn),
            triface({b.x,a.y,b.z},{a.x,b.y,b.z},{a.x,a.y,b.z},colors[1],roughn),
            triface({b.x,a.y,b.z},{a.x,b.y,b.z},b,colors[1],roughn),
            triface({a.x,a.y,b.z},{a.x,b.y,a.z},a,colors[2],roughn),
            triface({a.x,a.y,b.z},{a.x,b.y,a.z},{a.x,b.y,b.z},colors[2],roughn),
            triface({b.x,a.y,b.z},{b.x,b.y,a.z},{b.x,a.y,a.z},colors[3],roughn),
            triface({b.x,a.y,b.z},{b.x,b.y,a.z},b,colors[3],roughn),
            triface({b.x,a.y,b.z},a,{a.x,a.y,b.z},colors[4],roughn),
            triface({b.x,a.y,b.z},a,{b.x,a.y,a.z},colors[4],roughn),
            triface(b,{a.x,b.y,a.z},{a.x,b.y,b.z},colors[5],roughn),
            triface(b,{a.x,b.y,a.z},{b.x,b.y,a.z},colors[5],roughn)}){}
    };
    class sphere:public renderable{
    public:
        vector center;
        double radius,roughn,brightn;
        color_t color;
        sphere(const vector &center,const double &radius,const color_t &color,const double &roughn):center(center),radius(radius),color(color),roughn(roughn){}
        virtual std::shared_ptr<pickpoint_t> pick(const vector &pos,const vector &ray)const override{
            const auto cp=(pos-center);
            const double b=ray*cp,d=b*b-cp*cp+radius*radius;
            if(d<0) return nullptr;
            if(const double t=-b-sqrt(d);t>EPSILON) return std::make_shared<pickpoint_t>(pos+ray*t,(cp+ray*t).unit(),color,t,roughn);
            return nullptr;
        }
    };
    class renderer_guard{
    public:
        renderer *const rd;
        std::list<std::list<const renderable*>::iterator> fp;
        bool face_removed=false;
        template<typename objT,typename ctrT> renderer_guard(const collection<objT,ctrT> &ctr,renderer *const& rd):rd(rd){
            static_assert(std::is_base_of_v<renderable,objT>,"The elements of 'ctr' must implement the interface 'renderable' !");
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