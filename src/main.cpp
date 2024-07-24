#include"v3D.h"
#include<cfloat>
#include<functional>
#include<random>
#define clr(x) ege::color_t(ege::x)
#define cla(x,y) EGECOLORA(ege::x,y)
class render:public renderer3d{
public:
    float minfps=FLT_MAX,maxfps=-1,curfps=-1;
    double ltm=0,ctm=0;
    render(const int &xl,const int &yl):renderer3d({0,0,0},{0,0,400},{0,1,0},{1,0,0},xl,yl){}
    void init(const char *caption){
        ege::initgraph(width,height,ege::INIT_RENDERMANUAL);
        ege::setcaption(caption);
        ege::setbkmode(TRANSPARENT);
        ege::setfillcolor(ege::WHITE);
        ege::setfont(20,0,"Consolas");
    }
    void flush(const int &mp){
        ege::cleardevice();
        std::vector<std::future<void>> ft(mp);
        for(int i=0;i<mp;++i) ft[i]=std::async(std::launch::async,[&](const int &lx,const int &rx,const int &ly,const int &ry){render_pixel(lx,rx,ly,ry);},width/mp*(i),width/mp*(i+1),0,height);
        for(auto &p:ft) p.get();
        if((ctm=ege::fclock())-ltm>0.5){
            ltm=ctm;
            curfps=ege::getfps(),maxfps=std::max(maxfps,curfps),minfps=std::min(minfps,curfps);
        }
        ege::xyprintf(0,0,"FPS(MIN/CUR/MAX):%f/%f/%f",minfps,curfps,maxfps);
        ege::ege_fillrect(0,20,(curfps-minfps)/(maxfps-minfps)*width,20);
    }
    ~render(){ege::closegraph();}
};
void test(){
    std::random_device rdv;
    std::mt19937 gen(rdv());
    std::uniform_int_distribution<> dist(0,360);
    rect3d rect1({-50,50,450},{50,-50,550},{clr(BLUE),clr(RED),clr(YELLOW),clr(GREEN),clr(BROWN),clr(CYAN)}),rect1t=rect1,
          rect2({-50,350,450},{50,450,550},{clr(BLUE),clr(RED),clr(YELLOW),clr(GREEN),clr(BROWN),clr(CYAN)}),rect2t=rect2,r1,r2;
    render rd(1000,600);
    renderer3d_guard rg1(rect1,&rd),rg2(rect2,&rd),rgr1(r1,&rd),rgr2(r2,&rd);
    rd.init("v3D Collision Test");
    const auto c1=rect1.center();
    for(double dx=0,dy=0;ege::is_run();rect1=rect1t,rect2=rect2t){
        rect1.rotate({0,0,500},dist(gen),dist(gen),dist(gen));
        rect2.rotate({0,0,500},dist(gen),dist(gen),dist(gen));
        const auto c2=rect2.center();
        for(double tx=(c1.x-c2.x)/60,ty=(c1.y-c2.y)/60,tz=(c1.z-c2.z)/60,t=1;t<=120&&ege::is_run();){
            int mx,my;
            ege::mousepos(&mx,&my);
            dx=(rd.hh-my)*180.0/rd.hh,dy=(rd.hw-mx)*180.0/rd.hw;
            rect2+={tx,ty,tz};
            const auto pb1=rect1.box(),pb2=rect2.box();
            if(ege::keystate('C')){
                r1=rect3d(pb1.first,pb1.second,{cla(BLUE,100),cla(RED,100),cla(YELLOW,100),cla(GREEN,100),cla(BROWN,100),cla(CYAN,100)});
                r2=rect3d(pb2.first,pb2.second,{cla(BLUE,100),cla(RED,100),cla(YELLOW,100),cla(GREEN,100),cla(BROWN,100),cla(CYAN,100)});
            }
            else{
                r1=rect3d(pb1.first,pb1.second,{cla(BLUE,0),cla(RED,0),cla(YELLOW,0),cla(GREEN,0),cla(BROWN,0),cla(CYAN,0)});
                r2=rect3d(pb2.first,pb2.second,{cla(BLUE,0),cla(RED,0),cla(YELLOW,0),cla(GREEN,0),cla(BROWN,0),cla(CYAN,0)});
            }
            r1.rotate(c1,dx,dy,0),r2.rotate(c1,dx,dy,0);
            const auto t1=rect1,t2=rect2;
            rect1.rotate(c1,dx,dy,0),rect2.rotate(c1,dx,dy,0);
            rd.flush(std::max(std::thread::hardware_concurrency(),1u));
            ege::xyprintf(0,40,"colliding:%s",is_collided(pb1,pb2)?"true":"false");
            ege::xyprintf(0,60,"dx=%f dy=%f",dx,dy);
            ++t,ege::delay_ms(1),rect1=t1,rect2=t2;
        }
    }
}
int main(){
    test();
    return 0;
}
