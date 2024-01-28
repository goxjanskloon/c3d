#include"v3D.h"
class render:public renderer3d{
public:
    vector3d pos{0,0,0},facing{0,0,400},ud{0,0.5,0},rd{0.5,0,0};
    int xl=1000,yl=600,cx=xl>>1,cy=yl>>1;
    float minfps=__FLT_MAX__,maxfps=-1,curfps=-1;
    double ltm=0,ctm=0;
    render &init(){
        initgraph(xl,yl,getinitmode()|INIT_RENDERMANUAL);
        setcaption("v3D");
        ege_enable_aa(1);
        setbkmode(TRANSPARENT);
        setfont(20,0,"Consolas");
        return *this;
    }
    render &flush(){
        cleardevice();
        renderer3d::render(pos,facing,ud,rd,xl,yl);
        if((ctm=fclock())-ltm>0.5){
            ltm=ctm;
            curfps=getfps(),maxfps=std::max(maxfps,curfps),minfps=std::min(minfps,curfps);
        }
        xyprintf(0,0,"FPS(MIN/CUR/MAX):%f/%f/%f",minfps,curfps,maxfps);
        outtextxy(0,20,std::string(int(curfps/2),'#').c_str());
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
    for(double t=0;is_run();rect=rectt,t+=0.5,delay_fps(60)) rect.rotate(c,t,t,t),rd.flush();
}
int main(){
    test();
    return 0;
}
