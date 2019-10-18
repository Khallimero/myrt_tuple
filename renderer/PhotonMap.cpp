#include "PhotonMap.h"
#include "AutoLock.h"

PhotonMap::PhotonMap()
    :BinaryTree<PhotonMapElement>()
{
    setAutoDelete(true);
}

PhotonMap::~PhotonMap() {}

int PhotonMap::compareItems(const PhotonMapElement* t1,const PhotonMapElement* t2)const
{
    return t1->compareTo(t2);
}

void PhotonMap::addPhotonHit(int shapeId,const PhotonHit& ph)
{
    PhotonMapElement* c=insertShapeId(shapeId);
    c->getCollection().addPhotonHit(ph);
}

Collection<PhotonHit>* PhotonMap::addPhotonBox(int shapeId,const PhotonBox& pBox)
{
    PhotonMapElement* c=insertShapeId(shapeId);
    return c->getCollection().addPhotonBox(pBox);
}

PhotonMapElement* PhotonMap::insertShapeId(int shapeId)
{
    PhotonMapElement* e=new PhotonMapElement(shapeId);
    AutoLock autolock(this);
    PhotonMapElement* c=insert(e);
    autolock.unlock();
    if(c!=e)delete e;
    return c;
}

const Collection<PhotonHit>* PhotonMap::getPhotonHitCollection(int shapeId,const PhotonBox& pBox)
{
    PhotonMapElement e(shapeId);
    AutoLock autolock(this);
    PhotonMapElement* c=find(&e);
    autolock.unlock();
    if(c==NULL)return NULL;
    return c->getCollection().getPhotonHitCollection(pBox);
}

int PhotonMap::getNbPhotonHitCollection()
{
    int n=0;
    AutoLock autolock(this);
    for(PhotonMapElement* e=first(); e!=NULL; e=next())
        n+=e->getCollection().count();
    return n;
}
