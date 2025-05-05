#pragma once
#include<limits>
#include<numbers>
#include<random>
namespace c3d{
    constexpr float INF=std::numeric_limits<float>::infinity(),PI=std::numbers::pi_v<float>;
    static std::random_device random_device_;
    static std::mt19937 generator{random_device_()};
    inline float random_float(){
        static std::uniform_real_distribution<float> d(0,1);
        return d(generator);
    }
    inline float random_unit_float(){
        static std::uniform_real_distribution<float> d(-1,1);
        return d(generator);
    }
}