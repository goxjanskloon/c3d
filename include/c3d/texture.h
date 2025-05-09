#pragma once
#include<c3d/vector.h>
namespace c3d{
    class texture{
    public:
        virtual ~texture()=0;
        [[nodiscard]] virtual vector value(float u,float v,const vector &p)const=0;
    };
    class solid_texture final:public texture{
    public:
        vector color;
        [[nodiscard]] vector value(float u,float v,const vector &p)const override{
            return color;
        }
    };
}