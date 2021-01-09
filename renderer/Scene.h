#pragma once

#include "ShapeCollection.h"
#include "Collection.h"
#include "Shape.h"
#include "Light.h"
#include "Color.h"
#include "Hit.h"
#include "Ray.h"
#include "Point.h"
#include "Sphere.h"

class Scene
{
public:
    Scene();
    virtual ~Scene();

public:
    void addShape(SmartPointer<Shape> s);
    int getNbShapes()const;
    const Shape* getShape(int k)const;

    void addLight(SmartPointer<const Light> l);
    int getNbLights()const;
    const Light* getLight(int k)const;

    void setAmbiant(const Color& a)
    {
        this->ambiant=a;
    }
    const Color& getAmbiant()const
    {
        return ambiant;
    }

public:
    bool intersectShape(const Ray& r)const;
    Hit getHit(const Ray& r)const;
    ObjCollection<Hit> getHit(const ObjCollection<Ray>& r)const;
    ObjCollection<Hit> getIntersect(const ObjCollection<Ray>& r)const;
    double getDensity(const Point& p)const;

public:
    void setPhotonBoxIn(SmartPointer<const Sphere> s)
    {
        this->photonBoxIn=s;
    }
    const Sphere* getPhotonBoxIn()const
    {
        return photonBoxIn.getPointer();
    }
    void setPhotonBoxOut(SmartPointer<const Sphere> s)
    {
        this->photonBoxOut=s;
    }
    const Sphere* getPhotonBoxOut()const
    {
        return photonBoxOut.getPointer();
    }

public:
    unsigned int getSeed()const
    {
        return rseed;
    }
    void setSeed(unsigned int s)
    {
        this->rseed=s;
    }

protected:
    ShapeCollection shapes;
    ObjCollection< SmartPointer<const Light> > lights;
    Color ambiant;
    SmartPointer<const Sphere> photonBoxIn,photonBoxOut;

protected:
    unsigned int rseed;
};
