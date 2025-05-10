#pragma once
#include<c3d/hittable.h>
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
            return std::max(0.f,direction*uvw.w/PI);
        }
        [[nodiscard]] vector generate()const override{
            return uvw.transform(cosine_random_unit_vector());
        }
    };
    class hittable_pdf final:public pdf{
    public:
        const hittable &object;
        vector origin;
        hittable_pdf(const hittable &object,const vector &origin):object(object),origin(origin){}
        [[nodiscard]] float value(const vector &direction)const override{
            return object.pdf_value(origin,direction);
        }
        [[nodiscard]] vector generate()const override{
            return object.random(origin);
        }
    };
    class mixture_pdf final:public pdf{
    public:
        std::shared_ptr<pdf> a,b;
        mixture_pdf(const std::shared_ptr<pdf> &a,const std::shared_ptr<pdf> &b):a(a),b(b){}
        [[nodiscard]] float value(const vector &direction)const override{
            return (a->value(direction)+b->value(direction))/2.f;
        }
        [[nodiscard]] vector generate()const override{
            if(random_float()<.5f)
                return a->generate();
            return b->generate();
        }
    };

}