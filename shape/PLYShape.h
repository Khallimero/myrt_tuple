#pragma once

#include "Shape.h"
#include "Point.h"
#include "Vector.h"
#include "Collection.h"
#include "Sphere.h"
#include "Ray.h"
#include "Lockable.h"
#include "ShapeBuilder.h"

void* boxThread(void*);

class PLYShape:public Shape,public Lockable
{
    friend void* boxThread(void*);
    friend class ShapeBuilder<2>;

public:
    PLYShape(const char* f,double size,const Mark& mk=Mark::Ref);
    PLYShape(double size=1.0,const Mark& mk=Mark::Ref);
    virtual ~PLYShape();

public:
    virtual bool isInside(const Point& p,double e=0.0)const;

protected:
    struct PLYPrimitive
    {
        Point pt[3];
        Point b;
        Vector n;
    };

    struct PLYBox
    {
        const Sphere* s;
        Collection<const PLYPrimitive*> prm;
        Collection<const PLYPrimitive*> ht;
    };

    struct PLYLargeBox
    {
        Sphere* s;
        Collection<const PLYBox*> boxes;
    };

protected:
    virtual Hit _getHit(const Ray& r)const;
    virtual Hit __getHit(const Ray& r,const PLYPrimitive** p=NULL,const PLYBox** b=NULL)const;

protected:
    bool getNextBox(int* n);

protected:
    void buildFromFile(const char* filename);
    void buildBoxes(bool flgBox=true);
    void addPrimitive(const Point& a,const Point& b,const Point& c);
    void addBox(const Point& pt,double r);
    Point PLYtoRef(const Point& pt)const;

public:
    void setSmoothNormal(bool flg)
    {
        smoothNormal=flg;
    }
    const Shape* getBox()const
    {
        return box;
    }
    void setBox(const Shape* b)
    {
        this->box=b;
    }

protected:
    bool smoothNormal;
    Treble<double> size;
    const Shape* box;
    Collection<PLYPrimitive> shapes;
    Collection<PLYBox> boxes;
    Collection<PLYLargeBox*> largeBoxes;
    int nb_box;
};
