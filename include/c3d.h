#pragma once
#include<algorithm>
#include<execution>
#include<limits>
#include<memory>
#include<random>
#include<ranges>
namespace c3d{
    constexpr float INF=std::numeric_limits<float>::infinity(),PI=std::numbers::pi_v<float>;
    struct interval{
        float min,max;
        [[nodiscard]] float clamp(const float a)const{
            return a<min?min:a>max?max:a;
        }
        [[nodiscard]] bool contain(const float a)const{
            return min<=a&&a<=max;
        }
        [[nodiscard]] bool empty()const{
            return min>max;
        }
        void expand(const float a){
            min-=a;
            max+=a;
        }
        interval &intersect(const interval &a){
            if(a.min>min)
                min=a.min;
            if(a.max<max)
                max=a.max;
            return *this;
        }
        void unite(const interval &a){
            if(a.min<min)
                min=a.min;
            if(a.max>max)
                max=a.max;
        }
        [[nodiscard]] float length()const{
            return max-min;
        }
    };
    static constexpr interval universe_interval{-INF,INF},empty_interval{INF,-INF};
    inline interval unite(const interval &a,const interval &b){
        return{std::min(a.min,b.min),std::max(a.max,b.max)};
    }
    inline interval intersect(const interval &a,const interval &b){
        return{std::max(a.min,b.min),std::min(a.max,b.max)};
    }
    struct vector{
        float x,y,z;
        void rotate(const vector &origin,const vector &axis,float a);
        void rotate(const vector &axis,float a);
        void unitize();
    };
    inline vector operator+(const vector &a,const vector &b){
        return{a.x+b.x,a.y+b.y,a.z+b.z};
    }
    inline vector &operator+=(vector &a,const vector &b){
        return a.x+=b.x,a.y+=b.y,a.z+=b.z,a;
    }
    inline vector operator-(const vector &a,const vector &b){
        return{a.x-b.x,a.y-b.y,a.z-b.z};
    }
    inline vector &operator-=(vector &a,const vector &b){
        return a.x-=b.x,a.y-=b.y,a.z-=b.z,a;
    }
    inline vector operator*(const vector &v,const float a){
        return{v.x*a,v.y*a,v.z*a};
    }
    inline float operator*(const vector &a,const vector &b){
        return a.x*b.x+a.y*b.y+a.z*b.z;
    }
    inline vector operator&(const vector &a,const vector &b){
        return{a.y*b.z-a.z*b.y,a.z*b.x-a.x*b.z,a.x*b.y-a.y*b.x};
    }
    inline vector operator/(const vector &v,const float a){
        return{v.x/a,v.y/a,v.z/a};
    }
    inline vector &operator/=(vector &v,const float a){
        return v.x/=a,v.y/=a,v.z/=a,v;
    }
    inline vector operator-(const vector &v){
        return{-v.x,-v.y,-v.z};
    }
    inline float self_dot(const vector &v){
        return v.x*v.x+v.y*v.y+v.z*v.z;
    }
    inline float norm(const vector &v){
        return std::sqrt(self_dot(v));
    }
    inline vector unit(const vector &v){
        return v/norm(v);
    }
    inline vector rotate(const vector &v,const vector &axis,const float a){
        const float c=std::cos(a);return v*c+axis*(1-c)*(v*axis)+(v&axis)*std::sin(a);
    }
    inline vector rotate(const vector &v,const vector &origin,const vector &axis,const float a){
        return rotate(v-origin,axis,a)+origin;
    }
    inline bool operator==(const vector &a,const vector &b){
        return a.x==b.x&&a.y==b.y&&a.z==b.z;
    }
    inline void vector::rotate(const vector &origin,const vector &axis,const float a){
        *this-=origin;
        rotate(axis,a);
        *this+=origin;
    }
    inline void vector::rotate(const vector &axis,const float a){
        const float c=std::cos(a);
        *this=*this*c+axis*(1-c)*(*this*axis)+(*this&axis)*std::sin(a);
    }
    inline void vector::unitize(){
        *this/=norm(*this);
    }
    static std::uniform_real_distribution<float> unit_float_distribution(-1,1);
    static std::random_device random_device_;
    static std::mt19937 generator{random_device_()};
    vector random_unit(){
        const float b=unit_float_distribution(generator),r=std::sqrt(b*(b-1))*2,l=2*PI*unit_float_distribution(generator);
        return{std::cos(l)*r,std::sin(l)*r,1-2*b};
    }
    template<typename G> vector random_on_unit_hemisphere(G &g,const vector &n){
        const float b=unit_float_distribution(generator),r=std::sqrt(b*(b-1))*2,l=2*PI*unit_float_distribution(generator);
        const vector v{std::cos(l)*r,std::sin(l)*r,1-2*b};
        return v*n>0?v:-v;
    }
    struct ray{
        vector origin,direction;
        float time;
        [[nodiscard]] vector at(const float a)const{
            return origin+direction*a;
        }
    };
    struct aabb{
        interval x,y,z;
        aabb()=default;
        aabb(const interval &x,const interval &y,const interval &z):x(x),y(y),z(z){}
        aabb(const vector &a,const vector &b):x{std::min(a.x,b.x),std::max(a.x,b.x)},y{std::min(a.y,b.y),std::max(a.y,b.y)},z{std::min(a.z,b.z),std::max(a.z,b.z)}{}
        aabb(const aabb &a,const aabb &b):x(c3d::unite(a.x,b.x)),y(c3d::unite(a.y,b.y)),z(c3d::unite(a.z,b.z)){}
        [[nodiscard]] bool hit(const vector &origin,const vector &ray,interval interval_)const{
            return !(interval_.intersect({(x.min-origin.x)/ray.x,(x.max-origin.x)/ray.x}).empty()
                   ||interval_.intersect({(y.min-origin.y)/ray.y,(y.max-origin.y)/ray.y}).empty()
                   ||interval_.intersect({(z.min-origin.z)/ray.z,(z.max-origin.z)/ray.z}).empty());
        }
        void unite(const aabb &a){
            x.unite(a.x);
            y.unite(a.y);
            z.unite(a.z);
        }
    };
    const aabb empty_aabb{empty_interval,empty_interval,empty_interval};
    class material;
    struct hit_record{
        vector point,normal;
        float distance,u,v;
        std::shared_ptr<const material> material_;
    };
    class hittable{
    public:
        virtual ~hittable()=0;
        [[nodiscard]] virtual std::shared_ptr<hit_record> hit(const ray &ray_,const interval &interval_)const=0;
        [[nodiscard]] virtual aabb get_aabb()const=0;
    };
    class pdf{
    public:
        virtual ~pdf()=0;
        [[nodiscard]] virtual float value(const vector &direction)const=0;
        [[nodiscard]] virtual vector generate()const=0;
    };
    class sphere_pdf:public pdf{
    public:
        [[nodiscard]] float value(const vector &direction)const override{
            return 1/(4*PI);
        }
        [[nodiscard]] vector generate() const override {
            return random_unit();
        }
    };
    struct scatter_record{
        std::shared_ptr<pdf> pdf_;
        vector attenuation;
        ray skip_pdf;
    };
    class material{
    public:
        virtual ~material()=0;
        virtual vector emit(const ray &in,const hit_record &hit,float u,float v,const vector &p){
            return{.0f,.0f,.0f};
        }
        [[nodiscard]] virtual std::shared_ptr<scatter_record> scatter(const ray& in,const hit_record &hit)const=0;
    };
    class bvh_tree final:public hittable{
        bvh_tree(std::vector<std::shared_ptr<const hittable>> &objects,const int l,const int r):aabb_(){
            aabb_=empty_aabb;
            if(const std::size_t n=objects.size();n==1) {
                aabb_=(left=objects.front())->get_aabb();
                right=nullptr;
            } else if(n==2)
                aabb_={(left=objects.front())->get_aabb(),(right=objects.back())->get_aabb()};
            else {
                aabb_=objects.front()->get_aabb();
                for(auto p=std::next(objects.begin());p!=objects.end();++p)
                    aabb_.unite((*p)->get_aabb());
                const float xl=aabb_.x.length(),yl=aabb_.y.length(),zl=aabb_.z.length();
                const auto lp=objects.begin()+l,rp=objects.begin()+r;
                std::sort(std::execution::par_unseq,lp,rp,
                          [&](const std::shared_ptr<const hittable>&a,const std::shared_ptr<const hittable>&b) {
                              if(xl>yl&&xl>zl)
                                  return a->get_aabb().x.min<b->get_aabb().x.min;
                              if(yl>xl&&yl>zl)
                                  return a->get_aabb().y.min<b->get_aabb().y.min;
                              return a->get_aabb().z.min<b->get_aabb().z.min;
                          });
                const int m=l+r>>1;
                left=std::shared_ptr<hittable>(new bvh_tree(objects,l,m));
                right=std::shared_ptr<hittable>(new bvh_tree(objects,m+1,r));
            }
        }
    public:
        std::shared_ptr<const hittable> left,right;
        aabb aabb_;
        explicit bvh_tree(std::vector<std::shared_ptr<const hittable>> &objects):bvh_tree(objects,0,static_cast<int>(objects.size())-1){}
        [[nodiscard]] std::shared_ptr<hit_record> hit(const ray &ray_,const interval &interval_)const override{
            auto left_hit=left==nullptr?nullptr:left->hit(ray_,interval_),right_hit=right==nullptr?nullptr:right->hit(ray_,interval_);
            if(left_hit==nullptr) return right_hit;
            if(right_hit==nullptr) return left_hit;
            return left_hit->distance<right_hit->distance?left_hit:right_hit;
        }
        [[nodiscard]] aabb get_aabb()const override{
            return aabb_;
        }
    };
    class sphere final:public hittable{
    public:
        vector center,movement;
        float radius;
        std::shared_ptr<material> material_;
        [[nodiscard]] std::shared_ptr<hit_record> hit(const ray &ray_,const interval &interval_)const override{
            const vector c=center+movement*ray_.time,co=ray_.origin-c;
            const float b=ray_.direction*co,d=b*b-self_dot(co)+radius*radius;
            if(d<0)
                return nullptr;
            const float sd=std::sqrt(d);
            float t=-b-sd;
            if(!interval_.contain(t)){
                t+=sd*2;
                if(!interval_.contain(t))
                    return nullptr;
            }
            const vector p=ray_.at(t);
            return std::make_shared<hit_record>(hit_record{p,unit(p-c),t,(std::atan2(-p.z,p.x)+PI)/(2*PI),std::acos(-p.y)/PI,material_});
        }
        [[nodiscard]] aabb get_aabb()const override{
            aabb aabb_(center,center+movement);
            aabb_.x.expand(radius);
            aabb_.y.expand(radius);
            aabb_.z.expand(radius);
            return aabb_;
        }
    };
}