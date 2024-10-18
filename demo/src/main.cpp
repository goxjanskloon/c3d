#include "v3d/Renderable.hpp"
#include<cmath>
#include<cstring>
#include<fstream>
#include<iostream>
#include<v3d/v3d.hpp>
using namespace v3d;
using std::fmod,std::abs,std::min,std::ofstream,std::cerr,std::string;
/*class Ground:public Renderable{
    static constexpr Vector e1{1.0,0.0,0.0},e2{0.0,0.0,1.0},normal{0.0,1.0,0.0};
public:
    Real y;
    Color color;
    Ground(CR<Real> y,CR<Color> color):y(y),color(color){}
    Ptr<Point> pick(CR<Ray> ray)const override{
        const double det=e1*(ray.ray&e2);
        if(fabs(det)<v3d::EPSILON) return nullptr;
        if(const double t=e2*((ray.pos-Vector{0.0,y,0.0})&e1)/det;t>EPSILON){
            const auto p=ray.at(t);
            const double cx=fmod(abs(p.x),3.0),cz=fmod(abs(p.z),3.0);
            return makePtr<Point>(ray.at(t),normal,(p.x>=0?cx<=1.5:cx>1.5)^(p.z>=0?cz<=1.5:cz>1.5)?color:Vector{1.0,1.0,1.0}-color,Vector{},t,1.0);
        }
        return nullptr;
    }
};*/
unsigned int d2c(CR<Real> x){return min<unsigned int>(255u,x*255);}
int main(){
    constexpr Int WIDTH=600,HEIGHT=360,PGBAR_LEN=100;
    constexpr Color BLUE{0.0,0.0,1.0},RED{1.0,0.0,0.0},YELLOW{1.0,1.0,0.0},GREEN{0.0,1.0,0.0},BROWN{0.647,0.165,0.165},CYAN{0.0,1.0,1.0},WHITE{1.0,1.0,1.0};
    Renderer renderer(makePtr<BvhTree>(std::vector<Ptr<const Renderable>>{
        makePtr<Sphere>(Vector{0.0,0.0,0.0},4.0,BLUE,WHITE*0.2,0.7)
    }),{{0,0,-20.0},{0.0,0.0,300.0}},{0.0,1.0,0.0},{1.0,0.0,0.0},WIDTH,HEIGHT,{},1000);
    //renderer.emplace_back(new Sphere({-9.0,0.0,0.0},5.0,RED,{},1.0));
    //renderer.emplace_back(new Sphere({0.0,0.0,0.0},4.0,BLUE,WHITE*0.2,0.7));
    //renderer.emplace_back(new Sphere({9.0,0.0,0.0},5.0,BLUE,{},1.0));
    //renderer.emplace_back(new Ground(-4.0,CYAN));
    ofstream image("output.ppm");
    image<<"P3\n"<<WIDTH<<' '<<HEIGHT<<"\n255\n";
    cerr<<string(PGBAR_LEN+1,'-')<<"]\r[";
    for(int i=0,l=0;i<HEIGHT;++i)
        for(int j=0;j<WIDTH;++j){
            const auto c=renderer.render(j,i,8);
            image<<d2c(c.x)<<' '<<d2c(c.y)<<' '<<d2c(c.z)<<' ';
            if(int((i*HEIGHT+j)*1.0/(WIDTH*HEIGHT)*PGBAR_LEN)>l) ++l,cerr<<'=';
        }
    cerr<<'\n';
    return 0;
}