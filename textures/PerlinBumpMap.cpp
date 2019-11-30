#include "PerlinBumpMap.h"

PerlinBumpMap::PerlinBumpMap(double f):BumpMap(f) {}

PerlinBumpMap::~PerlinBumpMap() {}

Vector PerlinBumpMap::getVector(const Point& p)const
{
    struct bumpmap_unary_idx_op:public unary_idx_op<double,TREBLE_SIZE>
    {
        bumpmap_unary_idx_op(const PerlinNoise *pn,const Point& p):p(p)
        {
            this->pn=pn;
            this->o=pn->noise(p);
        }
        virtual double operator()(const tuple_idx<TREBLE_SIZE>& i)const
        {
            return pn->noise(p+Vector::UnitTab[i])-o;
        }

        const PerlinNoise *pn;
        const Point& p;
        double o;
    };

    return Vector(Tuple<double,TREBLE_SIZE>(bumpmap_unary_idx_op(&pn,p))*fct);
}
