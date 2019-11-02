#include "PhotonHitCollection.h"
#include "AutoLock.h"

PhotonHitCollection::PhotonHitCollection()
    :BinaryTree<PhotonHitCollectionElement>()
{
    setAutoDelete(true);
}

PhotonHitCollection::~PhotonHitCollection() {}

void PhotonHitCollection::addPhotonHit(const PhotonHit& ph)
{
    PhotonHitCollectionElement* c=addElement(new PhotonHitCollectionElement(ph.getPoint()));
    c->addPhotonHit(ph);
}

ObjCollection<PhotonHit>* PhotonHitCollection::addPhotonBox(const PhotonBox& pBox)
{
    return addElement(new PhotonHitCollectionElement(pBox))->getPhotonHits();
}

PhotonHitCollectionElement* PhotonHitCollection::addElement(PhotonHitCollectionElement* e)
{
    AutoLock autolock(this);
    PhotonHitCollectionElement* c=insert(e);
    autolock.unlock();
    if(c!=e)delete e;
    return c;
}

const ObjCollection<PhotonHit>* PhotonHitCollection::getPhotonHitCollection(const PhotonBox& pBox)
{
    PhotonHitCollectionElement e(pBox);
    AutoLock autolock(this);
    PhotonHitCollectionElement* c=find(&e);
    autolock.unlock();
    if(c==NULL)return NULL;
    return c->getPhotonHits();
}

void PhotonHitCollection::trim()
{
    AutoLock autolock(this);
    walk(&PhotonHitCollectionElement::trim);
}

int PhotonHitCollection::compareItems(const PhotonHitCollectionElement* t1,const PhotonHitCollectionElement* t2)const
{
    return t1->compareTo(t2);
}
