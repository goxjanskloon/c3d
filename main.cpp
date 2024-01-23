#include"v3D.h"
#include<iostream>
#include<random>
#define MT 1
inline bool keydown(int vKey){return GetAsyncKeyState(vKey)&0x8000;}
inline int genint(const int &l,const int &r){
    std::random_device rd;
    std::mt19937 gen(rd());
    return std::uniform_int_distribution<>(l,r)(gen);
}
class render:public renderer3d{
public:
    vector3d pos{0,0,0},facing{0,0,400},ud{0,1,0},rd{1,0,0};
#if MT
    static const
#endif
    int xl=500,yl=300,cx=xl>>1,cy=yl>>1;
    PIMAGE img=newimage(xl,yl);
#if MT
    std::array<std::pair<int,int>,xl*yl> pxs;
#endif
    render &init(){
#if MT
        for(int i=0;i<yl;i++)
            for(int j=0;j<xl;j++) pxs[i*xl+j]={i,j};
#endif
        initgraph(xl,yl,getinitmode());
        ege_enable_aa(1,img);
        return *this;
    }
    render &flush(){
#if MT
        renderer3d::render(pos,facing,ud,rd,xl,yl,pxs.begin(),pxs.end(),img);
#else
        renderer3d::render(pos,facing,ud,rd,xl,yl,img);
#endif
        cleardevice();
        putimage(0,0,img);
        cleardevice(img);
        return *this;
    }
};
void test(){
    rect3d rect({-50,50,450},{50,-50,550},{color_t(BLUE),color_t(RED),color_t(GREEN),color_t(YELLOW),color_t(BROWN),color_t(CYAN)}),rectt=rect;
    render rd;
    renderer3d_guard rg(rect,&rd);
    rd.init();
    auto c=rect.center();
    for(double t=0;is_run();rect=rectt,t+=0.5,Sleep(5)) rect.rotate(c,t,t,t),rd.flush();
}
int main(){
    test();
    return 0;
}
