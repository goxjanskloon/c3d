#define _USE_MATH_DEFINES
#include<cmath>
#include<cstring>
#include<fstream>
#include<iostream>
#include<v3d.hpp>
int main(){
    constexpr int WIDTH=400,HEIGHT=300;
    constexpr v3d::color_t BLUE=0x0000ff,RED=0xff0000,YELLOW=0xffff00,GREEN=0x00ff00,BROWN=0xa52a2a,CYAN=0x00ffff,BLACK=0u;
    std::array<v3d::color_t,6> colors;
    std::fill(colors.begin(),colors.end(),YELLOW);
    v3d::rect rect({-50.0,50.0,150.0},{50.0,-50.0,250.0},colors);
    v3d::renderer renderer({0.0,0.0,0.0},{0.0,0.0,150.0},{0.0,1.0,0.0},{1.0,0.0,0.0},WIDTH,HEIGHT,BLACK);
    v3d::renderer_guard rdg(rect,&renderer);
    std::ofstream image("cube.ppm");
    image<<"P3\n"<<WIDTH<<' '<<HEIGHT<<"\n255\n";
    for(int i=0;i<HEIGHT;++i)
        for(int j=0;j<WIDTH;++j){
            const auto color=renderer.render(j,i,0);
            image<<(color>>16&0xff)<<' '<<(color>>8&0xff)<<' '<<(color&0xff)<<' ';
        }
    image.close();
    return 0;
}