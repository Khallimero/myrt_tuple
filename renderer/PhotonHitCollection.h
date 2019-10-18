#pragma once

#include "BinaryTree.h"
#include "PhotonBox.h"
#include "PhotonHitCollectionElement.h"

class PhotonHitCollection:public BinaryTree<PhotonHitCollectionElement>
{
public:
    PhotonHitCollection();
    virtual ~PhotonHitCollection();

public:
    void addPhotonHit(const PhotonHit& ph);
    Collection<PhotonHit>* addPhotonBox(const PhotonBox& pBox);
    const Collection<PhotonHit>* getPhotonHitCollection(const PhotonBox& pBox);
    int getNbPhotonHits()const
    {
        return count();
    }
    void trim();

    virtual int compareItems(const PhotonHitCollectionElement* t1,const PhotonHitCollectionElement* t2)const;

protected:
    PhotonHitCollectionElement* addElement(PhotonHitCollectionElement* e);
};
