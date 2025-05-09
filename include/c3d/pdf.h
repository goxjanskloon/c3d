#pragma once
#include<c3d/onb.h>
#include<c3d/vector.h>
namespace c3d{
    class pdf{
    public:
        virtual ~pdf()=0;
        [[nodiscard]] virtual float value(const vector &direction)const=0;
        [[nodiscard]] virtual vector generate()const=0;
    };
    class sphere_pdf final:public pdf{
    public:
        [[nodiscard]] float value(const vector &direction)const override{
            return 1/(4*PI);
        }
        [[nodiscard]] vector generate()const override{
            return random_unit_vector();
        }
    };
    class cosine_pdf final:public pdf{
    public:
        onb uvw;
        explicit cosine_pdf(const vector &w):uvw(w){}
        [[nodiscard]] float value(const vector &direction)const override{
            return std::max(.0f,direction*uvw.w/PI);
        }
        [[nodiscard]] vector generate()const override{
            return uvw.transform(cosine_random_unit_vector());
        }
    };
}