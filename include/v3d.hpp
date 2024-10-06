#pragma once
#include<algorithm>
#include<array>
#include<cfloat>
#include<cmath>
#include<list>
#include<memory>
namespace v3d{
    using Real=double;
    using Int=int;
    using UInt=unsigned int;
    template<typename E> using Ptr=std::shared_ptr<E>;
    template<typename E> using List=std::list<E>;
    template<typename E,UInt L> using Array=std::array<E,L>;
    template<typename T> using CR=const T&;
    constexpr Real EPSILON=1e-10;
    template<typename E,typename... A> Ptr<E> makePtr(A&&... args){return std::make_shared<E>(args...);}
    class Ray;
    class Vector{
    public:
        Real x,y,z;
        constexpr Vector():x(0.0),y(0.0),z(0.0){};
        constexpr Vector(CR<Real> x,CR<Real> y,CR<Real> z):x(x),y(y),z(z){}
        Vector operator+(CR<Vector> v)const{return {x+v.x,y+v.y,z+v.z};}
        Vector &operator+=(CR<Vector> v){x+=v.x,y+=v.y,z+=v.z;return *this;}
        Vector operator-(CR<Vector> v)const{return {x-v.x,y-v.y,z-v.z};}
        Vector &operator-=(CR<Vector> v){x-=v.x,y-=v.y,z-=v.z;return *this;}
        Vector operator*(CR<Real> a)const{return {x*a,y*a,z*a};}
        Vector &operator*=(CR<Real> a){x*=a,y*=a,z*=a;return *this;}
        Real operator*(CR<Vector> v)const{return x*v.x+y*v.y+z*v.z;}
        Vector operator/(CR<Real> a)const{return {x/a,y/a,z/a};}
        Vector &operator/=(CR<Real> a){x/=a,y/=a,z/=a;return *this;}
        Vector operator&(CR<Vector> v)const{return {y*v.z-z*v.y,z*v.x-x*v.z,x*v.y-y*v.x};}
        Vector &rotate(CR<Ray> axis,CR<Real> angle);
        Real norm()const{return sqrt(x*x+y*y+z*z);}
        Vector unit()const{return operator/(norm());}
        Vector &unitize(){return operator/=(norm());}
    };
    using Color=Vector;
    class Ray{
    public:
        Vector pos,ray;
        Vector at(CR<Real> t)const{return pos+ray*t;}
    };
    Vector &Vector::rotate(CR<Ray> axis,CR<Real> angle){
        *this-=axis.ray;
        const Real cosa=cos(angle);
        return *this=*this*cosa+axis.ray*(1-cosa)*(axis.ray**this)+(axis.ray*sin(angle)&*this)+axis.pos;
    }
    template<typename E,typename C=std::list<E>> class Collection:public C{
    public:
        Collection()=default;
        explicit Collection(const C &objs):C(objs){}
        Collection &rotate(CR<Vector> base,CR<Vector> axis,CR<Real> angle){
            for(auto &p:*this) p.rotate(base,axis,angle);
            return *this;
        }
    };
    class Renderable{
    public:
        class Point{
        public:
            Vector point,normal;
            Color color;
            Real dist,roughn;
        };
        virtual ~Renderable()=default;
        virtual Ptr<Point> pick(CR<Ray> ray)const=0;
    };
    class Renderer:public List<Ptr<const Renderable>>{
    public:
        static constexpr UInt SSAA_SIZE=3,SSAA_COUNT=SSAA_SIZE*SSAA_SIZE;
        static constexpr Real SSAA_OFFSET[SSAA_SIZE]{-0.33,0.0,0.33};
        Vector ud,rd;
        Ray ray;
        UInt width,height;
        Color bgcolor;
        Renderer(CR<Ray> pos,CR<Vector> ud,CR<Vector> rd,CR<UInt> width,CR<UInt> height,CR<Color> bgcolor):ray(ray),ud(ud),rd(rd),width(width),height(height),bgcolor(bgcolor){}
        Color render(CR<Ray> ray,CR<UInt> rtd)const{
            Ptr<const Renderable::Point> point(nullptr);
            for(const auto &fp:*this)
                if(const auto t=fp->pick(ray);t.get()!=nullptr&&(point.get()==nullptr||t->dist<point->dist)) point=t;
            if(point.get()==nullptr) return {};
            const auto &[cpoint,normal,color,dist,roughn]=*point.get();
            if(rtd) return bgcolor+color*roughn+render({cpoint,ray.ray-normal*(ray.ray*normal)*2},rtd-1)*((ray.ray*-1.0)*normal)*(1.0-roughn);
            return bgcolor+color*roughn;
        }
        Color render_ssaa(CR<UInt> x,CR<UInt> y,CR<UInt> rtd)const{
            Color res;
            const auto hh=height>>1,hw=width>>1;
            for(UInt i=0;i<SSAA_SIZE;++i)
                for(UInt j=0;j<SSAA_SIZE;++j)
                    res+=render({ray.pos,(ray.ray+ud*(hh-y+SSAA_OFFSET[i])+rd*(x-hw+SSAA_OFFSET[j])).unit()},rtd);
            return res/=SSAA_COUNT;
        }
        Color render(CR<UInt> x,CR<UInt> y,CR<UInt> rtd)const{
            return render({ray.pos,(ray.ray+ud*((height>>1)-y)+rd*(x-(width>>1))).unit()},rtd);
        }
    };
    /*class TriFace:public Collection<Vector,Array<Vector,3>>,public Renderable{
    public:
        Color color;
        Real roughn;
        triface(const vector &v1,const vector &v2,const vector &v3,const light_t &light,CR<Real> roughn):collection({v1,v2,v3}),light(light),roughn(roughn){}
        std::shared_ptr<pickpoint_t> pick(const vector&pos,const vector&ray)const override{
            const auto e1=at(1)-at(0),e2=at(2)-at(0),pv=ray&e2;
            Real det=e1*pv;
            if(fabs(det)<EPSILON) return nullptr;
            det=1/det;
            const auto tvec=pos-at(0);
            const Real u=tvec*pv*det;
            if(u<0||u>1) return nullptr;
            const auto qv=tvec&e1;
            if(const Real v=ray*qv*det;v<0||u+v>1) return nullptr;
            if(const Real t=e2*qv*det;t>EPSILON) return std::make_shared<pickpoint_t>(pos+ray*t,(e1&e2).unit(),light,t,roughn);
            return nullptr;
        }
    };
    class rect:public collection<triface,std::array<triface,12>>{
    public:
        template<typename arrT> rect(const vector &a,const vector &b,const arrT &colors,CR<Real> roughn):collection({
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
    };*/
    class Sphere:public Renderable{
    public:
        Vector center;
        Real radius,roughn;
        Color color;
        Sphere(CR<Vector> center,CR<Real> radius,CR<Color> color,CR<Real> roughn):center(center),radius(radius),color(color),roughn(roughn){}
        Ptr<Point> pick(CR<Ray> ray)const override{
            const auto cp=(ray.pos-center);
            const Real b=ray.ray*cp,d=b*b-cp*cp+radius*radius;
            if(d<0) return nullptr;
            if(const Real t=-b-sqrt(d);t>EPSILON) return makePtr<Point>(ray.at(t),(cp+ray.ray*t).unit(),color,t,roughn);
            return nullptr;
        }
    };
    /*class renderer_guard{
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
    };*/
}