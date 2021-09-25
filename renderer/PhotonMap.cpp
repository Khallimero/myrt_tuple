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

ObjCollection<PhotonHit>* PhotonMap::addPhotonBox(int shapeId,const PhotonBox& pBox)
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

PhotonHitCollectionElement* PhotonMap::getPhotonHitCollectionElement(int shapeId,const PhotonBox& pBox)const
{
    PhotonMapElement e(shapeId);
    PhotonMapElement* c=cfind(&e);
    if(c==NULL)return NULL;
    return c->getCollection().getPhotonHitCollectionElement(pBox);
}

ObjCollection<PhotonHit>* PhotonMap::getPhotonHitCollection(int shapeId,const PhotonBox& pBox)const
{
    PhotonHitCollectionElement* phce=getPhotonHitCollectionElement(shapeId, pBox);
    if(phce==NULL)return NULL;
    return phce->getPhotonHits();
}

int PhotonMap::getNbPhotonHitCollection()
{
    int n=0;
    AutoLock autolock(this);
    for(PhotonMapElement* e=first(); e!=NULL; e=next())
        n+=e->getCollection().count();
    return n;
}
