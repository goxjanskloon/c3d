#pragma once
#ifdef _MSC_VER
#define NOMINMAX
#endif
#include<algorithm>
#include<array>
#include<vector>
#include<cfloat>
#include<cmath>
#include<list>
namespace v3d{
    inline double sind(const double &a){return sin(0.017453292519943295*a);}
    inline double cosd(const double &a){return cos(0.017453292519943295*a);}
    typedef unsigned int color_t;
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
        vector &rotate(const vector &c,const double &dx,const double &dy,const double &dz){
            *this-=c;
            const double sindx=sind(dx),sindy=sind(dy),sindz=sind(dz),cosdx=cosd(dx),cosdy=cosd(dy),cosdz=cosd(dz);
            auto [sx,sy,sz]=*this;
            y=y*cosdx-z*sindx;
            z=sy*sindx+z*cosdx;
            x=z*sindy-x*cosdy;
            z=z*cosdy+sx*sindy;
            sx=x;
            x=x*cosdz-y*sindz;
            y=sx*sindz+y*cosdz;
            *this+=c;
            return *this;
        }
        double norm()const{return sqrt(x*x+y*y+z*z);}
        const vector &center()const{return *this;}
        std::strong_ordering operator<=>(const vector&)const=default;
    };
    constexpr vector vector_null{DBL_MAX,DBL_MAX,DBL_MAX};
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
        vector center()const{
            vector c{0,0,0};
            for(const auto &p:*this) c+=p.center();
            return c/static_cast<double>(this->size());
        }
    };
    class renderable{
    public:
        virtual ~renderable()=default;
        struct pick_result_t{
            vector normal;
            color_t color;
            double dist;
            std::strong_ordering operator<=>(const pick_result_t&)const=default;
        };
        static constexpr pick_result_t pick_result_null{{DBL_MAX,DBL_MAX,DBL_MAX},UINT_MAX,DBL_MAX};
        virtual pick_result_t pick(const vector &pos,const vector &ray,const int &rtd)const{return pick_result_null;}
    };
    class renderer:public std::list<const renderable*>{
    public:
        vector pos,facing,ud,rd;
        int width,height;
        color_t bgcolor;
        renderer():pos(),facing(),ud(),rd(),width(),height(),bgcolor(){}
        renderer(const vector &pos,const vector &facing,const vector &ud,const vector &rd,const int &width,const int &height,const color_t &bgcolor):pos(pos),facing(facing),ud(ud),rd(rd),width(width),height(height),bgcolor(bgcolor){}
        color_t render(const int &x,const int &y,const int &rtd)const{
            const int hh=height>>1,hw=width>>1;
            typedef std::pair<double,color_t> pdc;
            std::vector<pdc> px;
            for(const auto &fp:*this)
                if(const auto t=fp->pick(pos,facing+ud*(hh-y)+rd*(x-hw),rtd);t!=renderable::pick_result_null) px.emplace_back(t.dist,t.color);
            if(px.empty()) return bgcolor;
            int mi=-1;
            for(int i=1;i<px.size();++i)
                if(px[i].first<px[mi].first) mi=i;
            if(mi==-1) return bgcolor;
            else return px[mi].second;
        }
    };
    class triface:public container<vector,std::array<vector,3>>,public renderable{
    public:
        color_t color;
        triface():color(){}
        triface(const vector &v1,const vector &v2,const vector &v3,const color_t &):container({v1,v2,v3}),color(color){}
        pick_result_t pick(const vector&pos,const vector&ray,const int&rtd)const override{
            const auto e1=at(1)-at(0),e2=at(2)-at(0),pv=ray&e2;
            double det=e1*pv;
            if(fabs(det)<DBL_EPSILON) return {-DBL_MAX,0};
            det=1/det;
            const auto tvec=pos-at(0);
            const double u=tvec*pv*det;
            if(u<0||u>1) return {-DBL_MAX,0};
            const auto qv=tvec&e1;
            if(const double v=ray*qv*det;v<0||u+v>1) return pick_result_null;
            if(const double t=e2*qv*det;t>0) return {e1&e2,color,t};
#pragma message("TODO:Raytracing")//TODO:Raytracing
            return pick_result_null;
        }
    };
    class rect:public container<triface,std::array<triface,12>>{
    public:
        rect()=default;
        rect(const vector &a,const vector &b,const color_t(&colors)[6]):container({
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
        std::pair<vector,vector> box()const{
            vector p1{DBL_MAX,-DBL_MAX,DBL_MAX},p2{-DBL_MAX,DBL_MAX,-DBL_MAX};
            for(const auto &f:*this)
                for(const auto &[x,y,z]:f) p1.x=std::min(p1.x,x),p1.y=std::max(p1.y,y),p1.z=std::min(p1.z,z),p2.x=std::max(p2.x,x),p2.y=std::min(p2.y,y),p2.z=std::max(p2.z,z);
            return{p1,p2};
        }
    };
    bool is_collided(const std::pair<vector,vector> &r1,const std::pair<vector,vector> &r2){
        const vector &p11=r1.first,&p12=r1.second,&p21=r2.first,&p22=r2.second;
        return (p21.x<p11.x&&p11.x<p22.x||p21.x<p12.x&&p12.x<p22.x||p11.x<p21.x&&p21.x<p12.x||p11.x<p22.x&&p22.x<p12.x)
            &&(p22.y<p11.y&&p11.y<p21.y||p22.y<p12.y&&p12.y<p21.y||p12.y<p21.y&&p21.y<p11.y||p12.y<p22.y&&p22.y<p11.y)
            &&(p21.z<p11.z&&p11.z<p22.z||p21.z<p12.z&&p12.z<p22.z||p11.z<p21.z&&p21.z<p12.z||p11.z<p22.z&&p22.z<p12.z);
    }
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