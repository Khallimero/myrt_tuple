#include "TextureBumpMap.h"

TextureBumpMap::TextureBumpMap(SmartPointer<const Texture> t,double f):BumpMap(f)
{
    this->t=t;
}

TextureBumpMap::~TextureBumpMap() {}

Vector TextureBumpMap::getVector(const Point& p)const
{
    struct texture_unary_idx_op:public unary_idx_op<double,TREBLE_SIZE>
    {
        texture_unary_idx_op(const Texture* t,const Point& p):p(p)
        {
            this->t=t;
            this->o=t->getColor(p).grey();
        }
        virtual double operator()(const tuple_idx<TREBLE_SIZE>& i)const
        {
            return t->getColor(p+Vector::UnitTab[i]).grey()-o;
        }

        double o;
        const Texture* t;
        const Point& p;
    };

    return Vector(Tuple<double,TREBLE_SIZE>(texture_unary_idx_op(t.getPointer(),p))*fct);
}
