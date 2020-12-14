#include "ShapeBuilder.h"
#include "BoxedCollectionShape.h"
#include "ShapeCollectionShape.h"
#include "ParametricSurfacePrimitive.h"
#include "Sphere.h"
#include "Cylinder.h"
#include "AddShape.h"
#include "SubShape.h"
#include "InterShape.h"
#include "Plan.h"
#include "PLYShape.h"

template <int N> TShapeBuilder<N>::TShapeBuilder(double size,double spin,const Tuple<int,N>& steps,double rad)
{
    this->size=size,this->spin=spin;
    this->steps=steps;
    this->rad=rad;
}

template <int N> TShapeBuilder<N>::~TShapeBuilder() {}

template <int N> NestedIterator<int,N> TShapeBuilder<N>::_getIterator()const
{
    struct iterator_unary_fct:unary_fct<int, NestedIterator<int,N> >
    {
        iterator_unary_fct(double spin)
        {
            this->spin=spin;
        }
        virtual void operator()(const int& i)
        {
            (**this).add(Iterator<int>((int)((double)i*spin)));
        }
        double spin;
    } _iterator_unary_fct(spin);

    return steps.tamper(_iterator_unary_fct);
}

template <int N> Tuple<double,N> TShapeBuilder<N>::_getAngle(const Tuple<int,N>& t)const
{
    static struct tuple_binary_op:public binary_op<int,double>
    {
        virtual double operator()(const volatile int& t1,const volatile int& t2)const
        {
            return (M_PI*2.0)*((double)t1/(double)t2);
        }
    } _binary_op;

    return Tuple<double,N>(t,steps,_binary_op);
}

template <int N> Point TShapeBuilder<N>::_getPoint(const Tuple<int,N>& i,const ParametricPrimitive<N> *prim,const Mark& mk)const
{
    return _getPoint(getPoint(i,prim),mk);
}

template <int N> Point TShapeBuilder<N>::_getPoint(const Point& p,const Mark& mk)const
{
    return mk.toRef(Point(p*size));
}

Shape* ShapeBuilder<1>::getBoxedInstance(const Mark& mk,
        const ParametricPrimitive<1> *prim,
        ShapeComponentType sType,
        double radius)const
{
    if(sType==Spherical)
    {
        BoxedCollectionShape *shape=new BoxedCollectionShape(radius<0?rad*5.:radius,mk);

        NestedIterator<int,1> it=_getIterator();
        while(it.next())
        {
            Tuple<int,1> _i=it.getTuple();
            Point o=_getPoint(_i,prim,mk);
            shape->addSphere(SmartPointer<Sphere>(new Sphere(getRad()*prim->getRad(_getAngle(_i)),Mark(o))));
        }

        shape->trim();
        shape->setBox(prim->getBox(this,mk));
        if(shape->getBox()==NULL)
        {
            double s=0.0;
            for(int i=0; i<shape->getShapes().count(); i++)
                s=MAX(s,mk.getOrig().dist(shape->getShapes().get(i)->getBox()->getMark().getOrig()));
            shape->setBox(SmartPointer<Shape>(new Sphere(s+rad+EPSILON,mk)));
        }

        return shape;
    }

    return getInstance(mk,prim,sType);
}

Shape* ShapeBuilder<1>::getInstance(const Mark& mk,
                                    const ParametricPrimitive<1> *prim,
                                    ShapeComponentType sType)const
{
    ShapeCollectionShape *shape=new ShapeCollectionShape(mk);

    NestedIterator<int,1> it=_getIterator();
    while(it.next())
    {
        Tuple<int,1> _i=it.getTuple();
        if(sType==Spherical)
        {
            Point o=_getPoint(_i,prim,mk);
            shape->getShapes()->_add(SmartPointer<Shape>(new Sphere(getRad()*prim->getRad(_getAngle(_i)),Mark(o))));
        }
        else if(sType==SubSpherical)
        {
            Point p0=_getPoint(_i-Tuple<int,1>::Unit,prim,mk);
            Point p1=_getPoint(_i,prim,mk);
            Point p2=_getPoint(_i+Tuple<int,1>::Unit,prim,mk);

            Shape *s1=new Sphere(getRad()*prim->getRad(_getAngle(_i-Tuple<int,1>::Unit)),Mark(p0));
            Shape *s2=new Sphere(getRad()*prim->getRad(_getAngle(_i)),Mark(p2));
            Shape *sp=new Sphere(getRad()*prim->getRad(_getAngle(_i+Tuple<int,1>::Unit)),Mark(p1));

            CSGShape *as=new AddShape(s1,s2);
            SubShape *ss=new SubShape(sp,as);
            ss->setHitShape(CSGShape::second,false);
            shape->getShapes()->_add(SmartPointer<Shape>(ss));
        }
        else if(sType==Cylindrical)
        {
            Point p0=_getPoint(_i-Tuple<int,1>::Unit,prim,mk);
            Point p1=_getPoint(_i,prim,mk);
            Point p2=_getPoint(_i+Tuple<int,1>::Unit,prim,mk);
            Point p3=_getPoint(_i+Tuple<int,1>::Unit*2,prim,mk);

            Vector v1=p0.getVectorTo(p1).norm()
                      +p1.getVectorTo(p2).norm();
            Vector v11=v1.getOrtho().norm();
            Vector v12=v1.prodVect(v11).norm();
            Vector v2=p1.getVectorTo(p2).norm()
                      +p2.getVectorTo(p3).norm();
            Vector v21=v2.getOrtho().norm();
            Vector v22=v2.prodVect(v21).norm();

            Shape *s1=new Plan(v11,v12,Mark(p1));
            Shape *s2=new Plan(v21,v22,Mark(p2));
            Shape *cyl=new Cylinder(getRad()*prim->getRad(_getAngle(_i)),Mark(p1,Rotation(p1.getVectorTo(p2))));

            Shape *ssp=new SubShape(s1,s2);
            InterShape *ss=new InterShape(cyl,ssp);
            ss->setHitShape(CSGShape::second,false);
            shape->getShapes()->_add(SmartPointer<Shape>(ss));
        }
    }

    shape->getShapes()->trim();
    shape->getShapes()->setBox(prim->getBox(this,mk));
    if(shape->getShapes()->getBox()==NULL)
    {
        double s=0.0;
        for(int i=0; i<shape->getShapes()->count(); i++)
            s=MAX(s,mk.getOrig().dist(shape->getShapes()->get(i)->getMark().getOrig()));
        shape->getShapes()->setBox(SmartPointer<Shape>(new Sphere(s+rad+EPSILON,mk)));
    }

    return shape;
}

Point ShapeBuilder<2>::__getPoint(const Tuple<int,2>& i,
                                  const ParametricPrimitive<1> *prim,
                                  const Mark& mk,
                                  double rad)const
{
    ParametricSurfacePrimitive psp(prim);
    Point p=psp.getPoint(_getAngle(i));
    Point o1=psp.getPoint(_getAngle(i-Pair<int>(1,0)));
    Point o2=psp.getPoint(_getAngle(i+Pair<int>(1,0)));
    Point p0=Ray(o1,o1.getVectorTo(o2)).proj(p);
    Point p1=p+p0.getVectorTo(p).norm()*rad;
    Point p2=p1.axialRotate(o1.getVectorTo(o2),p,_getAngle(i).get(IPair::second));
    return mk.toRef(Point(p2*size));
}

Shape* ShapeBuilder<2>::getInstance(const Mark& mk,
                                    const ParametricPrimitive<1> *prim,
                                    double rad,
                                    bool smoothNormal)const
{
    PLYShape *shape=new PLYShape();
    shape->setSmoothNormal(smoothNormal);

    NestedIterator<int,2> it=_getIterator();
    while(it.next())
    {
        Tuple<int,2> _i=it.getTuple();
        Point p0=__getPoint(_i,prim,mk,rad);

        NestedIterator<int,2> itp1;
        itp1.add(Iterator<int>(-1,1,2));
        itp1.add(Iterator<int>(0));
        while(itp1.next())
        {
            Tuple<int,2> i1=itp1.getTuple();
            NestedIterator<int,2> itp2;
            itp2.add(Iterator<int>(0));
            itp2.add(Iterator<int>(-1,1,2));
            while(itp2.next())
            {
                Tuple<int,2> i2=itp2.getTuple();
                shape->addPrimitive(p0,
                                    __getPoint(_i+i1,prim,mk,rad),
                                    __getPoint(_i+i2,prim,mk,rad));
            }
        }

        if(it[IPair::first].getId()%2==0&&it[IPair::second].getId()==0)
        {
            ParametricSurfacePrimitive psp(prim);
            Point p=_getPoint(_i,&psp,mk);
            Point p1=__getPoint(_i+Pair<int>(-1,0),prim,mk,rad);
            Point p2=__getPoint(_i+Pair<int>(1,0),prim,mk,rad);

            shape->addBox(p,MAX(p.dist(p1),p.dist(p2))+EPSILON);
        }
    }

    shape->buildBoxes(true);
    return shape;
}

Shape* ShapeBuilder<2>::getInstance(const Mark& mk,
                                    const ParametricPrimitive<2> *prim,
                                    bool smoothNormal)const
{
    PLYShape *shape=new PLYShape();
    shape->setSmoothNormal(smoothNormal);

    NestedIterator<int,2> it=_getIterator();
    while(it.next())
    {
        Tuple<int,2> _i=it.getTuple();
        Point p0=_getPoint(_i,prim,mk);

        NestedIterator<int,2> itp1;
        itp1.add(Iterator<int>(-1,1,2));
        itp1.add(Iterator<int>(0));
        while(itp1.next())
        {
            Tuple<int,2> i1=itp1.getTuple();
            NestedIterator<int,2> itp2;
            itp2.add(Iterator<int>(0));
            itp2.add(Iterator<int>(-1,1,2));
            while(itp2.next())
            {
                Tuple<int,2> i2=itp2.getTuple();
                shape->addPrimitive(p0,_getPoint(_i+i1,prim,mk),_getPoint(_i+i2,prim,mk));
            }
        }
    }

    shape->buildBoxes(true);
    return shape;
}

Shape* ShapeBuilder<2>::getInstance(const Mark& mk,
                                    const ParametricPrimitive<2> *prim,
                                    double rad)const
{
    ShapeCollectionShape *shape=new ShapeCollectionShape(mk);

    NestedIterator<int,2> it=_getIterator();
    while(it.next())
    {
        Tuple<int,2> _i=it.getTuple();
        Point p0=_getPoint(_i,prim,mk);

        shape->getShapes()->_add(new Sphere(rad,Mark(p0)));
    }

    return shape;
}

Point ShapeBuilder<2>::getPoint(const Tuple<int,2>& i,const ParametricPrimitive<2>* prim)const
{
    return prim->getPoint(_getAngle(i));
}

template class TShapeBuilder<1>;
template class TShapeBuilder<2>;
