#include "Shape.h"
#include "Sphere.h"

int Shape::_id=0;

Shape::Shape(const Mark& mk)
    :mark(mk)
{
    this->id=Shape::_id++;

    setTexture(NULL,Undefined);
    setBumpMap(NULL,Undefined);
    setCarveMap(NULL,Undefined);

    setDiffCoeff(0);
    setSpecCoeff(0);
    setGlareCoeff(0);
    setRadianceCoeff(0);
    setReflectCoeff(0);
    setRefractCoeff(0);
    setDensity(1);

    setShell(true);
    setBeerSizeCoeff(1);
    setBeerAbsorbCoeff(1);
    setBeerColor(Color::White);
}

Shape::~Shape() {}

bool Shape::operator==(const Shape& that)const
{
    return this->getId()==that.getId();
}

Point Shape::getRelativePoint(const Hit& h,Mapping mp)const
{
    return mark.fromRef(h.getPoint());
}

Color Shape::getColor(const Hit& h)const
{
    if(getTexture()==NULL)return Color::Black;
    return getTexture()->getColor(getRelativePoint(h,texMapping));
}

Hit Shape::getHit(const Ray& r)const
{
    Hit h=_getHit(r);
    return tamperHit(h,r);
}

ObjCollection<Hit> Shape::getHit(const ObjCollection<Ray>& r)const
{
    ObjCollection<Hit> hc(r._count());
    for(int i=0; i<r._count(); i++)
        hc._add(getHit(r[i]));
    return hc;
}

ObjCollection<Hit> Shape::getIntersect(const ObjCollection<Ray>& r)const
{
    return getHit(r);
}

Hit Shape::tamperHit(Hit& h, const Ray& r)const
{
    if(!h.isNull())
    {
        const Shape* s=h.getShape();
        const CarveMap* cm=s->getCarveMap();
        if(cm!=NULL&&cm->isCarved(s->getRelativePoint(h,s->getCarveMapMapping())*s->getCarveMapSize()))
            return getHit(Ray(h.getPoint()+r.getVector().norm()*EPSILON,r.getVector()));
        if(s->getBumpMap()!=NULL)
        {
            Vector n=h.getThNormal().norm()+s->getBumpMap()->getVector(s->getRelativePoint(h,s->getBumpMapMapping())*s->getBumpMapSize());
            if(n.cosAngle(h.getThNormal())<0)
                n=n.projOrthoTo(h.getThNormal());
            h.setNormal(n);
        }
    }
    return h;
}

bool Shape::intersect(const Ray& r)const
{
    return !(getHit(r).isNull());
}

bool Shape::isInside(const Point&,double)const
{
    return false;
}

void Shape::setTexture(SmartPointer<const Texture> tex,Mapping m,const Vector& textSize)
{
    setTexture(tex);
    setTextureMapping(m);
    setTextureSize(texSize);
}

void Shape::setBumpMap(SmartPointer<const BumpMap> bm,Mapping m,const Vector& bmSize)
{
    setBumpMap(bm);
    setBumpMapMapping(m);
    setBumpMapSize(bmSize);
}

void Shape::setCarveMap(SmartPointer<const CarveMap> crvMap,Mapping m,const Vector& cmSize)
{
    setCarveMap(crvMap);
    setCarveMapMapping(m);
    setCarveMapSize(cmSize);
}

void Shape::setBeer(double sizeCoeff,double absorbCoeff,const Color& beerColor)
{
    setShell(false);
    setBeerSizeCoeff(sizeCoeff);
    setBeerAbsorbCoeff(absorbCoeff);
    setBeerColor(beerColor);
}
