#pragma once
#include<c3d/vector.h>
namespace c3d{
    class camera{
    public:
        int width,height,samples_per_pixel,max_depth;
        float fov,defocus_angle,focus_distance;
        vector origin,direction,up,background_color;
    };
}