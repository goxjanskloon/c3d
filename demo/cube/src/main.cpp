#define _USE_MATH_DEFINES
#include<cmath>
#include<cstring>
#include<fstream>
#include<iostream>
#include<v3d.hpp>
using namespace v3d;
class Ground:public Renderable{
    static constexpr Vector e1{1.0,0.0,0.0},e2{0.0,0.0,1.0},normal{0.0,1.0,0.0};
public:
    Real y,roughn;
    Color color;
    Ground(CR<Real> y,CR<Real> roughn,CR<Color> color):y(y),roughn(roughn),color(color){}
    Ptr<Point> pick(CR<Ray> ray)const override{
        double det=e1*(ray.ray&e2);
        if(fabs(det)<v3d::EPSILON) return nullptr;
        if(const double t=e2*((ray.pos-Vector{0.0,y,0.0})&e1)/det;t>EPSILON) return makePtr<Point>(ray.at(t),normal,color,t,roughn);
        return nullptr;
    }
};
unsigned int d2c(CR<Real> x){return std::min<unsigned int>(255u,x);}
int main(){
    constexpr UInt WIDTH=1000u,HEIGHT=600u,PGBAR_LEN=100u;
    constexpr Color BLUE{0.0,0.0,255.0},RED{255.0,0.0,0.0},YELLOW{0.0,255.0,255.0},GREEN{0.0,255.0,0.0},BROWN{165.0,42.0,42.0},CYAN{0.0,255.0,255.0},WHITE{255.0,255.0,255.0};
    Renderer renderer({{0,0,-20.0},{0.0,0.0,300.0}},{0.0,1.0,0.0},{1.0,0.0,0.0},WIDTH,HEIGHT,WHITE*0.2);
    //renderer.emplace_back(new Sphere({-12.0,0.0,0.0},5.0,RED,0.9));
    renderer.emplace_back(new Ground(-10.0,1.0,CYAN));
    std::ofstream image("cube.ppm");
    image<<"P3\n"<<WIDTH<<' '<<HEIGHT<<"\n255\n";
    for(int i=0,l=0;i<HEIGHT;++i)
        for(int j=0;j<WIDTH;++j){
            const auto c=renderer.renderSsaa(j,i,8);
            image<<d2c(c.x)<<' '<<d2c(c.y)<<' '<<d2c(c.z)<<' ';
            if((i*HEIGHT+j)*PGBAR_LEN/(WIDTH*HEIGHT)>l) ++l,std::cerr<<'=';
        }
    return 0;
}