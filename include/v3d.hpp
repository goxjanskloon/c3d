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
    using color_t=unsigned int;
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
            vector normal;
            color_t color;
            double dist;
        };
        virtual std::shared_ptr<pickpoint_t> pick(const vector &pos,const vector &ray,const int &rtd)const{return nullptr;}
    };
    class renderer:public std::list<const renderable*>{
    public:
        static constexpr unsigned int SSAA_SIZE=3;
        static constexpr unsigned int SSAA_COUNT=SSAA_SIZE*SSAA_SIZE;
        static constexpr double SSAA_OFFSET[SSAA_SIZE]{-0.33,0.0,0.33};
        vector pos,facing,ud,rd;
        int width,height;
        color_t bgcolor;
        renderer():pos(),facing(),ud(),rd(),width(),height(),bgcolor(),std::list<const renderable*>(){}
        renderer(const vector &pos,const vector &facing,const vector &ud,const vector &rd,const int &width,const int &height,const color_t &bgcolor):pos(pos),facing(facing),ud(ud),rd(rd),width(width),height(height),bgcolor(bgcolor),std::list<const renderable*>(){}
        color_t render(const vector &ray,const int &rtd)const{
            using pdc=std::pair<double,color_t>;
            std::vector<pdc> px;
            for(const auto &fp:*this)
                if(const auto t=fp->pick(pos,ray,rtd);t.get()!=nullptr) px.emplace_back(t->dist,t->color);
            if(px.empty()) return bgcolor;
            int mi=0;
            for(int i=1;i<px.size();++i)
                if(px[i].first<px[mi].first) mi=i;
            return px[mi].second;
        }
        color_t render_ssaa(const int &x,const int &y,const int &rtd)const{
            unsigned int r=0u,g=0u,b=0u;
            const int hh=height>>1,hw=width>>1;
            for(int i=0;i<SSAA_SIZE;++i)
                for(int j=0;j<SSAA_SIZE;++j){
                    const auto c=render(facing+ud*(hh-y+SSAA_OFFSET[i])+rd*(x-hw+SSAA_OFFSET[j]),rtd);
                    r+=c>>16&0xff,g+=c>>8&0xff,b+=c&0xff;
                }
            return r/SSAA_COUNT<<16|g/SSAA_COUNT<<8|b/SSAA_COUNT;
        }
        color_t render(const int &x,const int &y,const int &rtd)const{
            using pdc=std::pair<double,color_t>;
            const int hh=height>>1,hw=width>>1;
            std::vector<pdc> px;
            for(const auto &fp:*this)
                if(const auto t=fp->pick(pos,facing+ud*(hh-y)+rd*(x-hw),rtd);t.get()!=nullptr) px.emplace_back(t->dist,t->color);
            if(px.empty()) return bgcolor;
            int mi=0;
            for(int i=1;i<px.size();++i)
                if(px[i].first<px[mi].first) mi=i;
            return px[mi].second;
        }
    };
    class triface:public collection<vector,std::array<vector,3>>,public renderable{
    public:
        color_t color;
        triface():color(){}
        triface(const vector &v1,const vector &v2,const vector &v3,const color_t &color):collection({v1,v2,v3}),color(color){}
        virtual std::shared_ptr<pickpoint_t> pick(const vector&pos,const vector&ray,const int&rtd)const override{
            const auto e1=at(1)-at(0),e2=at(2)-at(0),pv=ray&e2;
            double det=e1*pv;
            if(fabs(det)<DBL_EPSILON) return nullptr;
            det=1/det;
            const auto tvec=pos-at(0);
            const double u=tvec*pv*det;
            if(u<0||u>1) return nullptr;
            const auto qv=tvec&e1;
            if(const double v=ray*qv*det;v<0||u+v>1) return nullptr;
            if(const double t=e2*qv*det;t>0) return std::shared_ptr<pickpoint_t>{new pickpoint_t{e1&e2,color,t}};
            return nullptr;
        }
    };
    class rect:public collection<triface,std::array<triface,12>>{
    public:
        template<typename arrT> rect(const vector &a,const vector &b,const arrT &colors):collection({
            triface({b.x,a.y,a.z},{a.x,b.y,a.z},a,colors[0]),
            triface({b.x,a.y,a.z},{a.x,b.y,a.z},{b.x,b.y,a.z},colors[0]),
            triface({b.x,a.y,b.z},{a.x,b.y,b.z},{a.x,a.y,b.z},colors[1]),
            triface({b.x,a.y,b.z},{a.x,b.y,b.z},b,colors[1]),
            triface({a.x,a.y,b.z},{a.x,b.y,a.z},a,colors[2]),
            triface({a.x,a.y,b.z},{a.x,b.y,a.z},{a.x,b.y,b.z},colors[2]),
            triface({b.x,a.y,b.z},{b.x,b.y,a.z},{b.x,a.y,a.z},colors[3]),
            triface({b.x,a.y,b.z},{b.x,b.y,a.z},b,colors[3]),
            triface({b.x,a.y,b.z},a,{a.x,a.y,b.z},colors[4]),
            triface({b.x,a.y,b.z},a,{b.x,a.y,a.z},colors[4]),
            triface(b,{a.x,b.y,a.z},{a.x,b.y,b.z},colors[5]),
            triface(b,{a.x,b.y,a.z},{b.x,b.y,a.z},colors[5])}){}
    };
    class sphere:public renderable{
    public:
        vector center;
        double radius;
        color_t color;
        sphere(const vector &center,const double &radius,const color_t &color):center(center),radius(radius),color(color){}
        virtual std::shared_ptr<pickpoint_t> pick(const vector &pos,const vector &ray,const int &rtd)const override{
            const auto cp=(pos-center);
            const double b=ray*cp,d=b*b-cp*cp+radius*radius;
            if(d<0) return nullptr;
            const double t=(-b-sqrt(d))/pow(ray.norm(),2);
            return std::shared_ptr<pickpoint_t>{new pickpoint_t{(pos+ray*t-center).unit(),color,t}};
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