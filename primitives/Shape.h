#pragma once

#include "Point.h"
#include "Vector.h"
#include "Ray.h"
#include "Mark.h"
#include "Color.h"
#include "Texture.h"
#include "Hit.h"
#include "BumpMap.h"
#include "CarveMap.h"
#include "SmartPointer.h"
#include "Collection.h"

#include <stdlib.h>

enum Mapping
{
    Undefined=0,
    Planar,
    Volumic
};

class Shape
{
protected:
    Shape(const Mark& mk=Mark::Ref);

public:
    virtual ~Shape();

public:
    int getId()const
    {
        return id;
    }
    bool operator==(const Shape& that)const;

public:
    virtual Point getRelativePoint(const Hit& h,Mapping mp=Undefined)const;
    virtual Color getColor(const Hit& h)const;
    virtual Hit getHit(const Ray& r)const;
    virtual ObjCollection<Hit> getHit(const ObjCollection<Ray>& r)const;
    virtual ObjCollection<Hit> getIntersect(const ObjCollection<Ray>& r)const;
    virtual bool intersect(const Ray& r)const;
    virtual bool isInside(const Point&,double =0.0)const;

public:
    const Mark& getMark()const
    {
        return mark;
    }
    void setMark(const Mark& m)
    {
        mark=m;
    }
    const Texture* getTexture()const
    {
        return tex.getPointer();
    }
    void setTexture(SmartPointer<const Texture> tex)
    {
        this->tex=tex;
    };
    Mapping getTextureMapping()const
    {
        return texMapping;
    }
    void setTextureMapping(Mapping m)
    {
        this->texMapping=m;
    }
    const Vector& getTextureSize()const
    {
        return texSize;
    }
    void setTextureSize(const Vector& texSize)
    {
        this->texSize=texSize;
    }
    void setTexture(SmartPointer<const Texture> tex,Mapping m,const Vector& textSize=Vector::Unit);

    const BumpMap* getBumpMap()const
    {
        return bmpMap.getPointer();
    }
    void setBumpMap(SmartPointer<const BumpMap> bm)
    {
        this->bmpMap=bm;
    }
    Mapping getBumpMapMapping()const
    {
        return bmMapping;
    }
    void setBumpMapMapping(Mapping m)
    {
        this->bmMapping=m;
    }
    const Vector& getBumpMapSize()const
    {
        return bmSize;
    }
    void setBumpMapSize(const Vector& bmSize)
    {
        this->bmSize=bmSize;
    }
    void setBumpMap(SmartPointer<const BumpMap> bm,Mapping m,const Vector& bmSize=Vector::Unit);

    const CarveMap* getCarveMap()const
    {
        return crvMap.getPointer();
    }
    void setCarveMap(SmartPointer<const CarveMap> crvMap)
    {
        this->crvMap=crvMap;
    }
    Mapping getCarveMapMapping()const
    {
        return cmMapping;
    }
    void setCarveMapMapping(Mapping m)
    {
        this->cmMapping=m;
    }
    const Vector& getCarveMapSize()const
    {
        return cmSize;
    }
    void setCarveMapSize(const Vector& cmSize)
    {
        this->cmSize=cmSize;
    }
    void setCarveMap(SmartPointer<const CarveMap> crvMap,Mapping m,const Vector& cmSize=Vector::Unit);

    double getDiffCoeff()const
    {
        return diffCoeff;
    }
    void setDiffCoeff(double c)
    {
        this->diffCoeff=c;
    }
    double getSpecCoeff()const
    {
        return specCoeff;
    }
    void setSpecCoeff(double c)
    {
        this->specCoeff=c;
    }
    double getGlareCoeff()const
    {
        return glareCoeff;
    }
    void setGlareCoeff(double c)
    {
        this->glareCoeff=c;
    }
    double getRadianceCoeff()const
    {
        return radianceCoeff;
    }
    void setRadianceCoeff(double c)
    {
        this->radianceCoeff=c;
    }
    double getReflectCoeff()const
    {
        return reflectCoeff;
    }
    void setReflectCoeff(double c)
    {
        this->reflectCoeff=c;
    }
    double getRefractCoeff()const
    {
        return refractCoeff;
    }
    void setRefractCoeff(double c)
    {
        this->refractCoeff=c;
    }

    double getDensity()const
    {
        return density;
    }
    void setDensity(double d)
    {
        this->density=d;
    }

    bool isShell()const
    {
        return shell;
    }
    void setShell(bool s)
    {
        this->shell=s;
    }
    double getBeerSizeCoeff()const
    {
        return beerSizeCoeff;
    }
    void setBeerSizeCoeff(double b)
    {
        this->beerSizeCoeff=b;
    }
    double getBeerAbsorbCoeff()const
    {
        return beerAbsorbCoeff;
    }
    void setBeerAbsorbCoeff(double b)
    {
        this->beerAbsorbCoeff=b;
    }
    const Color& getBeerColor()const
    {
        return beerColor;
    }
    void setBeerColor(const Color& c)
    {
        this->beerColor=c;
    }
    void setBeer(double sizeCoeff,double absorbCoeff,const Color& beerColor=Color::White);

protected:
    virtual Hit _getHit(const Ray& r)const=0;
    virtual Hit tamperHit(Hit& h, const Ray& r)const;

protected:
    int id;

    Mark mark;

    SmartPointer<const Texture> tex;
    Mapping texMapping;
    Vector texSize;

    SmartPointer<const BumpMap> bmpMap;
    Mapping bmMapping;
    Vector bmSize;

    SmartPointer<const CarveMap> crvMap;
    Mapping cmMapping;
    Vector cmSize;

    double diffCoeff;
    double specCoeff;
    double glareCoeff;
    double radianceCoeff;
    double reflectCoeff;
    double refractCoeff;

    double density;

    bool shell;
    double beerSizeCoeff;
    double beerAbsorbCoeff;
    Color beerColor;

private:
    static int _id;
};
