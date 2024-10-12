#include "v3d/Vector.hpp"
#include<cmath>
#include<cstring>
#include<fstream>
#include<iostream>
#include<v3d/v3d.hpp>
using namespace v3d;
using std::fmod,std::abs,std::min,std::ofstream,std::cerr;
class Ground:public Renderable{
    static constexpr Vector e1{1.0,0.0,0.0},e2{0.0,0.0,1.0},normal{0.0,1.0,0.0};
public:
    Real y;
    Color color;
    Ground(CR<Real> y,CR<Real> roughn,CR<Color> color):y(y),color(color){}
    Ptr<Point> pick(CR<Ray> ray)const override{
        double det=e1*(ray.ray&e2);
        if(fabs(det)<v3d::EPSILON) return nullptr;
        if(const double t=e2*((ray.pos-Vector{0.0,y,0.0})&e1)/det;t>EPSILON) return makePtr<Point>(ray.at(t),normal,(fmod(abs(ray.at(t).x),10.0)<5.0)^(fmod(abs(ray.at(t).z),10.0)<5.0)?color:Vector{255.0,255.0,255.0}-color,Vector{},t);
        return nullptr;
    }
};
unsigned int d2c(CR<Real> x){return min<unsigned int>(255u,x*255);}
int main(){
    constexpr Int WIDTH=1000,HEIGHT=600,PGBAR_LEN=100;
    constexpr Color BLUE{0.0,0.0,1.0},RED{1.0,0.0,0.0},YELLOW{1.0,1.0,0.0},GREEN{0.0,1.0,0.0},BROWN{0.647,0.165,0.165},CYAN{0.0,1.0,1.0},WHITE{1.0,1.0,1.0};
    Renderer renderer({{0,0,-20.0},{0.0,0.0,300.0}},{0.0,1.0,0.0},{1.0,0.0,0.0},WIDTH,HEIGHT,{},20);
    renderer.emplace_back(new Sphere({-9.0,0.0,0.0},5.0,RED,{},0.9));
    renderer.emplace_back(new Sphere({0.0,0.0,0.0},4.0,YELLOW,YELLOW,0.0));
    //renderer.emplace_back(new Sphere({9.0,0.0,0.0},5.0,BROWN,{}));
    //renderer.emplace_back(new Ground(-8.0,0.7,CYAN));
    ofstream image("cube.ppm");
    cerr<<Vector::randomUnit().x;
    image<<"P3\n"<<WIDTH<<' '<<HEIGHT<<"\n255\n";
    for(int i=0,l=0;i<HEIGHT;++i)
        for(int j=0;j<WIDTH;++j){
            const auto c=renderer.render(j,i,8);
            image<<d2c(c.x)<<' '<<d2c(c.y)<<' '<<d2c(c.z)<<' ';
            if((i*HEIGHT+j)*PGBAR_LEN/(WIDTH*HEIGHT)>l) ++l,cerr<<'=';
        }
    cerr<<'\n';
    return 0;
}