#include<iostream>
#include<v3d.hpp>
int main(){
    const v3d::vector a{3.0,2.0,-3.0},b{3.0,-2.0,-4.0},c{-2.0,3.0,-2.0},ab=b-a,ac=c-a,x=ab&ac;
    const double s=x.norm()/2;
    std::cout<<s<<std::endl;
    return 0;
}