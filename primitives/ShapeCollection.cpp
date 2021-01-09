#include "ShapeCollection.h"

ShapeCollection::ShapeCollection(SmartPointer<Shape> b)
    :ObjCollection< SmartPointer<Shape> >()
{
    setBox(b);
}

ShapeCollection::~ShapeCollection() {}

Hit ShapeCollection::getHit(const Ray& r)const
{
    Hit h=Hit::null;
    if(box.getPointer()==NULL||!box->getHit(r).isNull())
    {
        double dMin=-1.0;
        for(int k=0; k<nb; k++)
        {
            Hit ht=tab[k]->getHit(r);
            if(!(ht.isNull()))
            {
                if(ht.getId()<0)ht.setId(k);
                double d=r.getPoint().dist(ht.getPoint());
                if((h.isNull())||(d<dMin))
                    dMin=d,h=ht;
            }
        }
    }

    return h;
}

ObjCollection<Hit> ShapeCollection::getHit(const ObjCollection<Ray>& r)const
{
    return _getHit(r,&Shape::getHit);
}

ObjCollection<Hit> ShapeCollection::getIntersect(const ObjCollection<Ray>& r)const
{
    return _getHit(r,&Shape::getIntersect);
}

ObjCollection<Hit> ShapeCollection::_getHit(const ObjCollection<Ray>& r,ObjCollection<Hit> (Shape::*fct)(const ObjCollection<Ray>&)const)const
{
    ObjCollection<Hit> hc(r._count());
    for(int i=0; i<r._count(); i++)
        hc._add(Hit::null);

    for(int k=0; k<nb; k++)
    {
        ObjCollection<Hit> h=(tab[k]->*fct)(r);
        for(int i=0; i<h._count(); i++)
        {
            if(!h[i].isNull())
                if(hc[i].isNull()||r[i].getPoint().dist(h[i].getPoint())<r[i].getPoint().dist(hc[i].getPoint()))
                    hc.getTab()[i]=h[i];
        }
    }

    return hc;
}

bool ShapeCollection::intersect(const Ray& r)const
{
    if(box.getPointer()==NULL||box->intersect(r))
    {
        for(int k=0; k<nb; k++)
        {
            if(tab[k]->intersect(r))
                return true;
        }
    }

    return false;
}

bool ShapeCollection::isInside(const Point& p,double e)const
{
    if(box.getPointer()==NULL||box->isInside(p,e))
    {
        for(int k=0; k<nb; k++)
        {
            if(tab[k]->isInside(p,e))
                return true;
        }
    }

    return false;
}
