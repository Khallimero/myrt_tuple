#include "Scene.h"

Scene::Scene()
{
    photonBoxIn=NULL,photonBoxOut=NULL;
}

Scene::~Scene() {}

void Scene::addShape(SmartPointer<Shape> s)
{
    shapes.add(s);
}

int Scene::getNbShapes()const
{
    return shapes._count();
}

const Shape* Scene::getShape(int k)const
{
    return shapes[k].getPointer();
}

void Scene::addLight(SmartPointer<const Light> l)
{
    lights.add(l);
}

int Scene::getNbLights()const
{
    return lights._count();
}

const Light* Scene::getLight(int k)const
{
    return lights[k].getPointer();
}

bool Scene::intersectShape(const Ray& r)const
{
    return shapes.intersect(r);
}

Hit Scene::getHit(const Ray& r)const
{
    return shapes.getHit(r);
}

double Scene::getDensity(const Point& p)const
{
    double d=1.0;
    for(int k=0; k<getNbShapes(); k++)
        if(shapes[k]->isInside(p))
            d*=shapes[k]->getDensity();
    return d;
}
