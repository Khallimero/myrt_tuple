#include "PhotonHitCollectionElement.h"
#include "AutoLock.h"

PhotonHitCollectionElement::PhotonHitCollectionElement(const PhotonBox& b)
    :pBox(b)
{}

PhotonHitCollectionElement::~PhotonHitCollectionElement() {}

int PhotonHitCollectionElement::compareTo(const PhotonHitCollectionElement* that)const
{
    return this->getBox().compareTo(that->getBox());
}

void PhotonHitCollectionElement::addPhotonHit(const PhotonHit& ph)
{
    AutoLock autolock(&pHits);
    for(int i=0; i<pHits._count(); i++)
    {
        if(ph==pHits.getTab()[i])
        {
            pHits.getTab()[i].addColor(ph.getColor());
            return;
        }
    }
    autolock.unlock();
    pHits.add(ph);
}

void PhotonHitCollectionElement::trim()
{
    pHits.trim();
}
