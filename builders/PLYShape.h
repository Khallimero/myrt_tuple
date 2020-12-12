#pragma once

#include "Shape.h"
#include "Point.h"
#include "Vector.h"
#include "Collection.h"
#include "Sphere.h"
#include "Ray.h"
#include "Lockable.h"
#include "SmartPointer.h"
#include "ShapeBuilder.h"
#include "OpenCLContext.h"
#include "OpenCLKernel.h"

#ifndef OpenCL
void* boxThread(void*);
#endif

class PLYShape:public Shape,public Lockable
{
#ifndef OpenCL
    friend void* boxThread(void*);
#endif
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
        bool operator==(const PLYPrimitive& that)
        {
            return false;
        }
    };

#ifndef OpenCL
    struct PLYBox
    {
        const Sphere* box;
        Collection<const PLYPrimitive*> prm;
        Collection<const PLYPrimitive*> ht;
        bool operator==(const PLYBox& that)
        {
            return this->box==that.box;
        }
    };

    struct PLYLargeBox
    {
        Sphere* box;
        Collection<const PLYBox*> boxes;
    };
#endif

protected:
    virtual Hit _getHit(const Ray& r)const;
#ifdef OpenCL
    virtual Hit __getHit(const Ray& r)const;
#else
    virtual Hit __getHit(const Ray& r,const PLYPrimitive** p=NULL,const PLYBox** b=NULL)const;
#endif

#ifndef OpenCL
protected:
    bool getNextBox(int* n);
    void addBox(const Point& pt,double r);
#endif

protected:
    void buildFromFile(const char* filename);
    void buildBoxes(bool flgBox=true);
    void addPrimitive(const Point& a,const Point& b,const Point& c);
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
    Treble<double> size;
    bool smoothNormal;
    const Shape* box;
    ObjCollection<PLYPrimitive> shapes;
#ifdef OpenCL
    SmartPointer<OpenCLContext> context;
    SmartPointer<OpenCLKernel> hit_kernel,nrm_kernel;
#else
    ObjCollection<PLYBox> boxes;
    Collection<PLYLargeBox*> largeBoxes;
    int nb_box;
#endif
};
