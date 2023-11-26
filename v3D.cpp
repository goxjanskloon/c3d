#include"v3D.h"
#include<vector>
#include<cfloat>
vector3d &vector3d::rotate(const vector3d &c,const double &dx,const double &dy,const double &dz){
    x-=c.x,y-=c.y,z-=c.z;
    const double sindx=sind(dx),sindy=sind(dy),sindz=sind(dz),cosdx=cosd(dx),cosdy=cosd(dy),cosdz=cosd(dz);
    auto s=*this;
    y=y*cosdx-z*sindx;
    z=s.y*sindx+z*cosdx;
    x=z*sindy-x*cosdy;
    z=z*cosdy+s.x*sindy;
    s.x=x;
    x=x*cosdz-y*sindz;
    y=s.x*sindz+y*cosdz;
    x+=c.x,y+=c.y,z+=c.z;
    return *this;
}
void render3d::render(const vector3d &pos,const vector3d &facing,const vector3d &ud,const vector3d &ld,const int &width,const int &height,const PIMAGE &pimg){
    auto mid=pos+facing;
    for(int i=0;i<height;i++)
        for(int j=0;j<width;j++){
            std::list<std::pair<double,color_t>> px;
            for(auto &f:*this){
                vector3d e1=(*f)[1]-(*f)[0],e2=(*f)[2]-(*f)[0],t,d=facing+ud*(i-(height>>1))+ld*(j-(width>>1)),p=d&e2;
                double det=e1*p;
                if(det>0) t=pos-(*f)[0];
                else t=(*f)[0]-pos,det*=-1;
                if(det<0.0001f) continue;
                auto u=t*p;if(u<0||u>det) continue;
                auto q=t&e1;double v=d*q;
                if(v<0||u+v>det) continue;
                double dt=e2*q;if(dt<0) continue;
                px.emplace_back(dt,f->color);
            }
            px.sort([](const std::pair<double,color_t> &x,const std::pair<double,color_t> &y){return x.first<y.first;});
            color_t pxc=0;
            for(auto &p:px){
                putpixel_f(height-1-j,width-1-i,p.second,pimg);
                if(EGEGET_A(p.second)==0xff) break;
            }
        }
}
rect3d::rect3d(const vector3d &a,const vector3d &b,const color_t(&colors)[6]):contnr3d({
    triface3d({b.x,a.y,a.z},{a.x,b.y,a.z},{a.x,a.y,a.z},colors[0]),
    triface3d({b.x,a.y,a.z},{a.x,b.y,a.z},{b.x,b.y,a.z},colors[0]),
    triface3d({b.x,a.y,b.z},{a.x,b.y,b.z},{a.x,a.y,b.z},colors[1]),
    triface3d({b.x,a.y,b.z},{a.x,b.y,b.z},{b.x,b.y,b.z},colors[1]),
    triface3d({a.x,a.y,b.z},{a.x,b.y,a.z},{a.x,a.y,a.z},colors[2]),
    triface3d({a.x,a.y,b.z},{a.x,b.y,a.z},{a.x,b.y,b.z},colors[2]),
    triface3d({b.x,a.y,b.z},{b.x,b.y,a.z},{b.x,a.y,a.z},colors[3]),
    triface3d({b.x,a.y,b.z},{b.x,b.y,a.z},{b.x,b.y,b.z},colors[3]),
    triface3d({b.x,a.y,b.z},{a.x,a.y,a.z},{a.x,a.y,b.z},colors[4]),
    triface3d({b.x,a.y,b.z},{a.x,a.y,a.z},{b.x,a.y,a.z},colors[4]),
    triface3d({b.x,b.y,b.z},{a.x,b.y,a.z},{a.x,b.y,b.z},colors[5]),
    triface3d({b.x,b.y,b.z},{a.x,b.y,a.z},{b.x,b.y,a.z},colors[5])}){}
std::pair<vector3d,vector3d> rect3d::collisionbox()const{
    vector3d p1{DBL_MAX,-DBL_MAX,DBL_MAX},p2{-DBL_MAX,DBL_MAX,-DBL_MAX};
    for(const auto &f:*this)
        for(const auto &p:f) p1.x=std::min(p1.x,p.x),p1.y=std::max(p1.y,p.y),p1.z=std::min(p1.z,p.z),p2.x=std::max(p2.x,p.x),p2.y=std::min(p2.y,p.y),p2.z=std::max(p2.z,p.z);
    return std::make_pair(p1,p2);
}
byte colliding(const std::pair<vector3d,vector3d> &r1,const std::pair<vector3d,vector3d> &r2){
    const vector3d &p11=r1.first,&p12=r1.second,&p21=r2.first,&p22=r2.second;
    byte c=((p21.x<p11.x&&p11.x<p22.x||p21.x<p12.x&&p12.x<p22.x||p11.x<p21.x&&p21.x<p12.x||p11.x<p22.x&&p22.x<p12.x)<<2
           |(p22.y<p11.y&&p11.y<p21.y||p22.y<p12.y&&p12.y<p21.y||p12.y<p21.y&&p21.y<p11.y||p12.y<p22.y&&p22.y<p11.y)<<1
           |(p21.z<p11.z&&p11.z<p22.z||p21.z<p12.z&&p12.z<p22.z||p11.z<p21.z&&p21.z<p12.z||p11.z<p22.z&&p22.z<p12.z)<<0);
    return c|((c>>2&1)&(c>>1&1)&(c&1))<<3;
}
