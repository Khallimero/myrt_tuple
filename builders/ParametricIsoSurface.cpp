#include "ParametricIsoSurface.h"
#include "NestedIterator.h"

#include <math.h>

ParametricIsoSurface::ParametricIsoSurface(const ParametricPrimitive<1> *prim,
        double size,double spin,int steps,double rad,
        const Mark& mk)
    :StraightShapeBuilder(1.0,spin,steps,rad/size),IsoSurface(size,NULL,mk)
{
    this->prim=prim;

    double dMax=0.0;
    NestedIterator<int,1> it=_getIterator();
    while(it.next())
    {
        Tuple<int,1> _i=it.getTuple();
        Point o=_getPoint(_i,prim);
        points._add(o);
        dMax=MAX(dMax,o.dist(_getPoint(_i+Tuple<int,1>::Unit,prim)));
    }

    double boxRad=sqrt(Arithmetic< Pair<double> >(Pair<double>(getRad(),dMax)))+EPSILON;
    StraightShapeBuilder ssb(1.0,spin,steps,boxRad);
    IsoSurface::box=ssb.getInstance(Mark::Ref,prim,Spherical);
}

ParametricIsoSurface::~ParametricIsoSurface() {}

double ParametricIsoSurface::getValue(const Point& p)const
{
    double dMin=getRad();
    int iMin=-1;

    for(int i=0; i<points._count(); i++)
    {
        double d=p.dist(points[i])-getRad();
        if(d<dMin)dMin=d,iMin=i;
    }

    if(iMin!=-1)
    {
        Tuple<double,1> t1=_getAngle(Tuple<int,1>::Unit*(iMin-1));
        double d1=p.dist(_getPoint(prim->getPoint(t1)));
        Tuple<double,1> t2=_getAngle(Tuple<int,1>::Unit*(iMin+1));
        double d2=p.dist(_getPoint(prim->getPoint(t2)));

        while(fabs((d2-d1)/get(Tuple<double,1>(t2-t1)))>ISO_THRESHOLD)
        {
            Tuple<double,1> t0=Tuple<double,1>(t1+t2)/2.0;
            double d0=p.dist(_getPoint(prim->getPoint(t0)));
            if(fabs(d2-d0)>fabs(d1-d0))t2=t0,d2=d0;
            else t1=t0,d1=d0;
            dMin=d0-getRad();
        }
    }

    return dMin;
}
