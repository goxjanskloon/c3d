#include"v3D.h"
#include<functional>
#include<random>
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
    void init(const char *caption){
        initgraph(width,height,INIT_RENDERMANUAL);
        setcaption(caption);
        setbkmode(TRANSPARENT);
        setfillcolor(WHITE);
        setfont(20,0,"Consolas");
    }
    void flush(){
        cleardevice();
        for(int i=1;i<4;i++)for(int j=1;j<4;j++) ft[i-1][j-1]=std::async(std::launch::async,rdfunc,w[i-1],w[i],h[j-1],h[j]);
        for(int i=0;i<3;i++)for(int j=0;j<3;j++) ft[i][j].get();
        if((ctm=fclock())-ltm>0.5){
            ltm=ctm;
            curfps=getfps(),maxfps=std::max(maxfps,curfps),minfps=std::min(minfps,curfps);
        }
        xyprintf(0,0,"FPS(MIN/CUR/MAX):%f/%f/%f",minfps,curfps,maxfps);
        ege_fillrect(0,20,(curfps-minfps)/(maxfps-minfps)*width,20);
    }
    ~render(){closegraph();}
};
void test(){
    std::random_device rdv;
    std::mt19937 gen(rdv());
    std::uniform_int_distribution<> dist(0,360);
    rect3d rect1({-50,50,450},{50,-50,550},{color_t(BLUE),color_t(RED),color_t(YELLOW),color_t(GREEN),color_t(BROWN),color_t(CYAN)}),rect1t=rect1,
          rect2({-50,350,450},{50,450,550},{color_t(BLUE),color_t(RED),color_t(YELLOW),color_t(GREEN),color_t(BROWN),color_t(CYAN)}),rect2t=rect2,r1,r2;
    render rd(1000,600);
    renderer3d_guard rg1(rect1,&rd),rg2(rect2,&rd),rgr1(r1,&rd),rgr2(r2,&rd);
    rd.init("v3D Collision Test");
    const auto c1=rect1.center();
    for(double dx=0,dy=0;is_run();rect1=rect1t,rect2=rect2t){
        rect1.rotate({0,0,500},dist(gen),dist(gen),dist(gen));
        rect2.rotate({0,0,500},dist(gen),dist(gen),dist(gen));
        const auto c2=rect2.center();
        for(double tx=(c1.x-c2.x)/60,ty=(c1.y-c2.y)/60,tz=(c1.z-c2.z)/60,t=1;t<=120&&is_run();){
            int mx,my;
            mousepos(&mx,&my);
            dx=(rd.hh-my)*180.0/rd.hh,dy=(rd.hw-mx)*180.0/rd.hw;
            rect2+={tx,ty,tz};
            const auto pb1=rect1.collisionbox(),pb2=rect2.collisionbox();
            if(keystate('C')){
                r1=rect3d(pb1.first,pb1.second,{EGECOLORA(BLUE,100),EGECOLORA(RED,100),EGECOLORA(YELLOW,100),EGECOLORA(GREEN,100),EGECOLORA(BROWN,100),EGECOLORA(CYAN,100)});
                r2=rect3d(pb2.first,pb2.second,{EGECOLORA(BLUE,100),EGECOLORA(RED,100),EGECOLORA(YELLOW,100),EGECOLORA(GREEN,100),EGECOLORA(BROWN,100),EGECOLORA(CYAN,100)});
            }
            else{
                r1=rect3d(pb1.first,pb1.second,{EGECOLORA(BLUE,0),EGECOLORA(RED,0),EGECOLORA(YELLOW,0),EGECOLORA(GREEN,0),EGECOLORA(BROWN,0),EGECOLORA(CYAN,0)});
                r2=rect3d(pb2.first,pb2.second,{EGECOLORA(BLUE,0),EGECOLORA(RED,0),EGECOLORA(YELLOW,0),EGECOLORA(GREEN,0),EGECOLORA(BROWN,0),EGECOLORA(CYAN,0)});
            }
            r1.rotate(c1,dx,dy,0),r2.rotate(c1,dx,dy,0);
            const auto t1=rect1,t2=rect2;
            rect1.rotate(c1,dx,dy,0),rect2.rotate(c1,dx,dy,0);
            const byte c=colliding(pb1,pb2);
            rd.flush();
            xyprintf(0,40,"colliding:%s",(c>>3)?"true":"false");
            xyprintf(0,60,"dx=%f dy=%f",dx,dy);
            t++,delay_ms(1),rect1=t1,rect2=t2;
        }
    }
}
int main(){
    test();
    return 0;
}
