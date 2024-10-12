#pragma once
#include<array>
#include<cfloat>
#include<list>
#include<memory>
#include<random>
namespace v3d{
    class Vector;
    using Real=double;
    using Int=int;
    template<typename E> using Ptr=std::shared_ptr<E>;
    template<typename E> using List=std::list<E>;
    template<typename E,Int L> using Array=std::array<E,L>;
    template<typename T> using CR=const T&;
    using RandomDevice=std::random_device;
    using RandomEngine=std::default_random_engine;
    using RealDist=std::uniform_real_distribution<double>;
    using Color=Vector;
    constexpr Real EPSILON=1e-10,REAL_MAX=1e300;
    template<typename E,typename... A> Ptr<E> makePtr(A&&... args){return std::make_shared<E>(args...);}
}