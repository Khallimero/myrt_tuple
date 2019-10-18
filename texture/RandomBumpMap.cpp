#include "RandomBumpMap.h"

RandomBumpMap::RandomBumpMap(int w,int h,double f)
    :BumpMap(f),m(w,h)
{
    for(int i=0; i<m.width(); i++)
    {
        for(int j=0; j<m.height(); j++)
            m[i][j]=Vector::random()*fct;
    }
}

RandomBumpMap::~RandomBumpMap() {}

Vector RandomBumpMap::getVector(const Point& p)const
{
    return m.getElem(p);
}
