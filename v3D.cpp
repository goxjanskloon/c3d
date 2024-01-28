#include"v3D.h"
#include<utility>
#include<vector>
vector3d &vector3d::rotate(const vector3d &c,const double &dx,const double &dy,const double &dz){
    *this-=c;
    const double sindx=sind(dx),sindy=sind(dy),sindz=sind(dz),cosdx=cosd(dx),cosdy=cosd(dy),cosdz=cosd(dz);
    auto s=*this;
    y=y*cosdx-z*sindx;
    z=s.y*sindx+z*cosdx;
    x=z*sindy-x*cosdy;
    z=z*cosdy+s.x*sindy;
    s.x=x;
    x=x*cosdz-y*sindz;
    y=s.x*sindz+y*cosdz;
    *this+=c;
    return *this;
}
void renderer3d::render(const vector3d &pos,const vector3d &facing,const vector3d &ud,const vector3d &rd,const int &width,const int &height){
    const auto mid=pos+facing;
    const int hh=height>>1,hw=width>>1,h[4]{0,height*0.3,height*0.6,height},w[4]{0,width*0.3,width*0.6,width};
    const auto f=[&](const int &lx,const int &rx,const int &ly,const int &ry){
    for(int i=ly;i<ry;i++)for(int j=lx;j<rx;j++){
        std::list<std::pair<double,color_t>> px;
        for(auto &fp:*this){
            auto &f=*fp;
            const auto e1=f[1]-f[0],e2=f[2]-f[0],d=facing+ud*(hh-i)+rd*(j-hw),pvec=d&e2;
            double det=e1*pvec;
            if(fabs(det)<__DBL_EPSILON__) continue;
            det=1/det;
            const auto tvec=pos-f[0];
            const double u=tvec*pvec*det;
            if(u<0||u>1) continue;
            const auto qvec=tvec&e1;
            const double v=(d*qvec)*det;
            if(v<0||u+v>1) continue;
            const double t=e2*qvec*det;
            if(t>0) px.emplace_back(t,f.color);
        }
        px.sort([](const std::pair<double,color_t> &x,const std::pair<double,color_t> &y){return x.first<y.first;});
        for(auto &p:px){
            putpixel_f(j,i,p.second);
            if(EGEGET_A(p.second)==0xff) break;
    }}};
    for(int i=1;i<4;i++)
        for(int j=1;j<4;j++) ft[i-1][j-1]=std::async(std::launch::async,f,w[i-1],w[i],h[j-1],h[j]);
    for(int i=0;i<3;i++)
        for(int j=0;j<3;j++) ft[i][j].get();
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
