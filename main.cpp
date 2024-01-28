#include"v3D.h"
class render:public renderer3d{
public:
    vector3d pos{0,0,0},facing{0,0,400},ud{0,1,0},rd{1,0,0};
    int xl=1000,yl=600,cx=xl>>1,cy=yl>>1;
    PIMAGE img=newimage(xl,yl);
    render &init(){
        initgraph(xl,yl,getinitmode());
        return *this;
    }
    render &flush(){
        renderer3d::render(pos,facing,ud,rd,xl,yl,img);
        cleardevice();
        putimage(0,0,img);
        cleardevice(img);
        return *this;
    }
    ~render(){closegraph();}
};
void test(){
    rect3d rect({-50,50,450},{50,-50,550},{color_t(BLUE),color_t(RED),color_t(GREEN),color_t(YELLOW),color_t(BROWN),color_t(CYAN)}),rectt=rect;
    render rd;
    renderer3d_guard rg(rect,&rd);
    rd.init();
    const auto c=rect.center();
    for(double t=0;is_run();rect=rectt,t+=0.5,Sleep(5)) rect.rotate(c,t,t,t),rd.flush();
}
int main(){
    test();
    return 0;
}
