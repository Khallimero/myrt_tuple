#pragma once

#include "Collection.h"
#include "PhotonHit.h"
#include "PhotonBox.h"

class PhotonHitCollectionElement
{
public:
    PhotonHitCollectionElement(const PhotonBox& b);
    virtual ~PhotonHitCollectionElement();

public:
    const PhotonBox& getBox()const
    {
        return pBox;
    }
    ObjCollection<PhotonHit>* getPhotonHits()
    {
        return &pHits;
    }

public:
    virtual int compareTo(const PhotonHitCollectionElement* that)const;
    void addPhotonHit(const PhotonHit& ph);
    void trim();

protected:
    PhotonBox pBox;
    ObjCollection<PhotonHit> pHits;
};
