#include"v3D.h"
#include<random>
inline bool keydown(int vKey){ return GetAsyncKeyState(vKey)&0x8000; }
inline int genint(const int &l,const int &r){
    std::random_device rd;
    std::mt19937 gen(rd());
    return std::uniform_int_distribution<>(l,r)(gen);
}
class render:public render3d{
public:
    vector3d pos{0,0,100},facing{0,0,400},ud{0,1,0},rd{1,0,0};
    int xl=1000,yl=600,cx=xl>>1,cy=yl>>1;
    PIMAGE img=newimage(xl,yl);
    render &init(){
        initgraph(xl,yl,getinitmode());
        ege_enable_aa(1,img);
        setbkmode(TRANSPARENT,img);
        setfont(20,0,"Consolas",img);
        setcolor(WHITE,img);
        return *this;
    }
    render &flush(){
        render3d::render(pos,facing,ud,rd,xl,yl,img);
        cleardevice();
        putimage(0,0,img);
        cleardevice(img);
        return *this;
    }
};
void test(){
    rect3d rect({-50,50,450},{50,-50,550},{color_t(BLUE),color_t(RED),color_t(YELLOW),color_t(GREEN),color_t(BROWN),color_t(CYAN)}),rectt=rect;
    render rd;
    render3d_guard rg(rect,&rd);
    rd.init();
    auto c=rect.center();
    for(double t=0;!keydown(27);rect=rectt,t+=0.5,Sleep(5)) rect.rotate(c,t,t,t),rd.flush();
}
int main(){
    test();
    return 0;
}
