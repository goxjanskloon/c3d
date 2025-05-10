#include<c3d/camera.h>
#include<c3d/sphere.h>
int main(){
    c3d::vector red{1.f,0.f,0.f};
    const std::shared_ptr<c3d::lambertian> st{};
    c3d::sphere a{c3d::vector{-1.f,0.f,0.f},c3d::vector{0.f,0.f,0.f},1.f,std::make_shared<c3d::lambertian>(std::make_shared<c3d::solid_texture>(c3d::vector{1.f,0.f,0.f}))};
    return 0;
}