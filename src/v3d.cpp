#include"v3d.hpp"
#include<algorithm>
#include<cfloat>
#include<utility>
#include<vector>
namespace v3d{
vector &vector::rotate(const vector &c,const double &dx,const double &dy,const double &dz){
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
std::pair<double,ege::color_t> triface::pick(const vector &pos,const vector &ray,const int &rtd)const{
    const auto e1=at(1)-at(0),e2=at(2)-at(0),pv=ray&e2;
    double det=e1*pv;
    if(fabs(det)<DBL_EPSILON) return {-DBL_MAX,0};
    det=1/det;
    const auto tvec=pos-at(0);
    const double u=tvec*pv*det;
    if(u<0||u>1) return {-DBL_MAX,0};
    const auto qv=tvec&e1;
    if(const double v=ray*qv*det;v<0||u+v>1) return {-DBL_MAX,0};
    if(const double t=e2*qv*det;t>0) return {t,color};
    return {-DBL_MAX,0};
}
ege::color_t renderer::render(const int &x,const int &y,const int &rtd)const{
    std::vector<std::pair<double,ege::color_t>> px;
    for(const auto &fp:*this){
        auto t=fp->pick(pos,facing+ud*(hh-y)+rd*(x-hw),rtd);
        if(t.first!=-DBL_MAX) px.emplace_back(t);
    }
    if(px.empty()) return EGECOLORA(ege::BLACK,0);
    std::sort(px.begin(),px.end(),[](const std::pair<double,ege::color_t> &a,const std::pair<double,ege::color_t> &b){return a.first<b.first;});
    int l=0;while(l<px.size()-1&&EGEGET_A(px[l].second)!=255) ++l;
    ege::color_t res=EGECOLORA(ege::BLACK,0);
    for(;l>=0;--l)if(EGEGET_A(px[l].second))res=ege::alphablend(res,px[l].second);
    return res;
}
rect::rect(const vector &a,const vector &b,const ege::color_t(&colors)[6]):container({
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
std::pair<vector,vector> rect::box()const{
    vector p1{DBL_MAX,-DBL_MAX,DBL_MAX},p2{-DBL_MAX,DBL_MAX,-DBL_MAX};
    for(const auto &f:*this)
        for(const auto &[x,y,z]:f) p1.x=std::min(p1.x,x),p1.y=std::max(p1.y,y),p1.z=std::min(p1.z,z),p2.x=std::max(p2.x,x),p2.y=std::min(p2.y,y),p2.z=std::max(p2.z,z);
    return{p1,p2};
}
bool is_collided(const std::pair<vector,vector> &r1,const std::pair<vector,vector> &r2){
    const vector &p11=r1.first,&p12=r1.second,&p21=r2.first,&p22=r2.second;
    return (p21.x<p11.x&&p11.x<p22.x||p21.x<p12.x&&p12.x<p22.x||p11.x<p21.x&&p21.x<p12.x||p11.x<p22.x&&p22.x<p12.x)
         &&(p22.y<p11.y&&p11.y<p21.y||p22.y<p12.y&&p12.y<p21.y||p12.y<p21.y&&p21.y<p11.y||p12.y<p22.y&&p22.y<p11.y)
         &&(p21.z<p11.z&&p11.z<p22.z||p21.z<p12.z&&p12.z<p22.z||p11.z<p21.z&&p21.z<p12.z||p11.z<p22.z&&p22.z<p12.z);
}
}