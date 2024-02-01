#include"v3D.h"
#include<functional>
class render:public renderer3d{
public:
    int h[4]{},w[4]{};
    float minfps=__FLT_MAX__,maxfps=-1,curfps=-1;
    double ltm=0,ctm=0;
    std::future<void> ft[3][3];
    const std::function<void(const int&,const int&,const int&,const int&)> rdfunc=std::bind(render_pixel,this,std::placeholders::_1,std::placeholders::_2,std::placeholders::_3,std::placeholders::_4);
    render(const int &xl,const int &yl):renderer3d({0,0,0},{0,0,400},{0,1,0},{1,0,0},xl,yl){
        h[1]=height*0.3,h[2]=height*0.6,h[3]=height;
        w[1]=width*0.3,w[2]=width*0.6,w[3]=width;
    }
    render &init(){
        initgraph(width,height,INIT_RENDERMANUAL);
        setcaption("v3D");
        setbkmode(TRANSPARENT);
        setfont(20,0,"Consolas");
        setcolor(WHITE);
        return *this;
    }
    render &flush(){
        cleardevice();
        for(int i=1;i<4;i++)for(int j=1;j<4;j++) ft[i-1][j-1]=std::async(std::launch::async,rdfunc,w[i-1],w[i],h[j-1],h[j]);
        for(int i=0;i<3;i++)for(int j=0;j<3;j++) ft[i][j].get();
        if((ctm=fclock())-ltm>0.5){
            ltm=ctm;
            curfps=getfps(),maxfps=std::max(maxfps,curfps),minfps=std::min(minfps,curfps);
        }
        xyprintf(0,0,"FPS(MIN/CUR/MAX):%f/%f/%f",minfps,curfps,maxfps);
        ege_fillrect(0,20,int(curfps/2),20);
        return *this;
    }
    ~render(){closegraph();}
};
void test(){
    rect3d rect({-100,100,450},{100,-100,650},{color_t(BLUE),color_t(RED),color_t(GREEN),color_t(YELLOW),color_t(BROWN),color_t(CYAN)}),rectt=rect;
    render rd(1000,600);
    renderer3d_guard rg(rect,&rd);
    rd.init();
    const auto c=rect.center();
    for(double t=0;is_run();rect=rectt,t+=0.5,delay_fps(60)) rect.rotate(c,t*1.5,t*2,t),rd.flush();
}
int main(){
    test();
    return 0;
}
