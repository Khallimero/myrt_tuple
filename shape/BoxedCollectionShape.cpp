#include "BoxedCollectionShape.h"

BoxedCollectionShape::BoxedCollectionShape(double r,const Mark& mk)
    :Shape(mk)
{
    radius=r;
}

BoxedCollectionShape::~BoxedCollectionShape()
{
    for(int i=0; i<shapes.count(); i++)
    {
        delete shapes.getTab()[i];
        shapes.getTab()[i]=NULL;
    }
}

void BoxedCollectionShape::trim()
{
    shapes.trim();
    for(int i=0; i<shapes.count(); i++)
        shapes[i]->trim();
}

Hit BoxedCollectionShape::_getHit(const Ray& r)const
{
    Hit h=Hit::null;
    if(box.getPointer()==NULL||!box->getHit(r).isNull())
    {
        double d=-1;
        for(int i=0; i<shapes.count(); i++)
        {
            Hit hTmp=shapes[i]->getHit(r);
            if(!hTmp.isNull())
            {
                double dTmp=r.getPoint().dist(hTmp.getPoint());
                if(h.isNull()||dTmp<d)
                    h=hTmp,d=dTmp;
            }
        }
    }

    if(!h.isNull())
        h.setShape(this);
    return h;
}

bool BoxedCollectionShape::isInside(const Point& p,double e)const
{
    if(box.getPointer()==NULL||box->isInside(p,e))
        for(int i=0; i<shapes.count(); i++)
            if(shapes[i]->isInside(p,e))
                return true;
    return false;
}

void BoxedCollectionShape::addSphere(SmartPointer<Sphere> s)
{
    for(int i=0; i<shapes.count(); i++)
    {
        if(shapes[i]->getBox()->getMark().getOrig().dist(s->getMark().getOrig())+s->getRadius()<radius)
        {
            shapes[i]->add(SmartPointer<Shape>(s.detach()));
            return;
        }
    }

    ShapeCollection *col=new ShapeCollection(SmartPointer<Shape>(new Sphere(MAX(radius,s->getRadius()+EPSILON),s->getMark())));
    col->add(SmartPointer<Shape>(s.detach()));
    shapes.add(col);
}
