#define _USE_MATH_DEFINES
#include<cmath>
#include<cstring>
#include<fstream>
#include<iostream>
#include<vector>
#include<v3d.hpp>
class ground_t:public v3d::renderable{
    static constexpr v3d::vector e1{1.0,0.0,0.0},e2{0.0,0.0,1.0},normal{0.0,1.0,0.0};
public:
    double y,roughn;
    v3d::color_t color;
    ground_t(const double &y,const v3d::color_t &color,const double &roughn):y(y),color(color),roughn(roughn){}
    virtual std::shared_ptr<pickpoint_t> pick(const v3d::vector &pos,const v3d::vector &ray)const override{
        double det=e1*(ray&e2);
        if(fabs(det)<v3d::EPSILON) return nullptr;
        if(const double t=e2*((pos-v3d::vector{0.0,y,0.0})&e1)/det;t>v3d::EPSILON) return std::make_shared<pickpoint_t>(pos+ray*t,normal,color,t,roughn);
        return nullptr;
    }
};
int main(){
    constexpr int WIDTH=1000,HEIGHT=600,PGBAR_LEN=100;
    constexpr v3d::color_t BLUE=0x0000ffu,RED=0xff0000u,YELLOW=0xffff00u,GREEN=0x00ff00u,BROWN=0xa52a2au,CYAN=0x00ffffu,BLACK=0u;
    v3d::color_t colors[]{BLUE,RED,YELLOW,GREEN,BROWN,CYAN};
    v3d::rect rect({-5.0,5.0,-5.0},{5.0,-5.0,5.0},colors,0.9);
    rect.rotate({0.0,0.0,0.0},{0.0,1.0,0.0},M_PI_4*1.2);
    rect.rotate({0.0,0.0,0.0},{1.0,0.0,0.0},-M_PI_4*0.7);
    v3d::renderer renderer({-5.0,-2.0,-20.0},{0.0,0.0,300.0},{0.0,1.0,0.0},{1.0,0.0,0.0},WIDTH,HEIGHT,0u);
    v3d::renderer_guard rgr(rect,&renderer);
    renderer.emplace_back(new v3d::sphere({-12.0,0.0,-3.0},5,RED,0.9));
    //renderer.emplace_back(new ground_t(-9.0,0xabcdefu,1.0));
    std::ofstream image("cube.ppm");
    image<<"P3\n"<<WIDTH<<' '<<HEIGHT<<"\n255\n";
    for(int i=0,l=0;i<HEIGHT;++i)
        for(int j=0;j<WIDTH;++j){
            const auto c=renderer.render_ssaa(j,i,8);
            image<<v3d::color_r(c)<<' '<<v3d::color_g(c)<<' '<<v3d::color_b(c)<<' ';
            if((i*HEIGHT+j)*PGBAR_LEN/(WIDTH*HEIGHT)>l) ++l,std::cerr<<'=';
        }
    return 0;
}