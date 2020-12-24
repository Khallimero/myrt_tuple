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
#include "OpenCLKernel.h"

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
    virtual bool intersect(const Ray& r)const;
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

protected:
    virtual Hit _getHit(const Ray& r)const;
    virtual Hit __getHit(const Ray& r,bool intersect=false,const PLYPrimitive** p=NULL,const PLYBox** b=NULL,int *bufferId=NULL)const;

protected:
    bool getNextBox(int* n);
    void addBox(const Point& pt,double r);

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
    SmartPointer<OpenCLKernel> hit_kernel,nrm_kernel;
    Collection<int> box_buffId;
    int hit_buffId[3],nrm_buffId[3];
#endif
    ObjCollection<PLYBox> boxes;
    Collection<PLYLargeBox*> largeBoxes;
    int nb_box;
};
