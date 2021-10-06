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
    PLYShape(const char* f,bool smooth,double size,const Mark& mk=Mark::Ref);
    PLYShape(bool smooth,double size=1.0,const Mark& mk=Mark::Ref);
    virtual ~PLYShape();

public:
    virtual bool isInside(const Point& p,double e=0.0)const;
    virtual ObjCollection<Hit> getHit(const ObjCollection<Ray>& r)const;
    virtual ObjCollection<Hit> getIntersect(const ObjCollection<Ray>& r)const;

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
        Sphere* box;
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
        int cntHt;
    };

protected:
    virtual Hit _getHit(const Ray& r)const;
    ObjCollection<Hit> _getHit(const ObjCollection<Ray>& r)const;
    ObjCollection<Hit> __getHit(const ObjCollection<Ray>& r,const PLYPrimitive*** p=NULL,const PLYBox*** b=NULL)const;
    void _addHit(const ObjCollection<Ray>& r,ObjCollection<Hit>& hc,int k,const PLYBox* box,int id,const PLYPrimitive*** p=NULL,const PLYBox*** b=NULL)const;
#ifdef OpenCL
    void _runHitKernel(int nbShapes, const ObjCollection<Ray>& r,ObjCollection<Hit>& hc,int* bCnt,const PLYPrimitive*** p,const PLYBox*** b)const;
#endif

protected:
    bool getNextBox(int* n);
    void addBox(const Point& pt,double r);

protected:
    void buildFromFile(const char* filename);
    void buildBoxes(bool flgBox=true);
    void addPrimitive(const Point& a,const Point& b,const Point& c);
    Point PLYtoRef(const Point& pt)const;

public:
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
    SmartPointer<OpenCLKernel> adj_kernel,hit_kernel;
    mutable int adj_buffId[6],hit_buffId[4];
    mutable int nb_ray,nb_hit;
#endif
    ObjCollection<PLYBox> boxes;
    Collection<PLYLargeBox*> largeBoxes;
    int nb_box;
};
