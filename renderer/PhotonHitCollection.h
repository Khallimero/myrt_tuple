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
    ObjCollection<PhotonHit>* addPhotonBox(const PhotonBox& pBox);
    PhotonHitCollectionElement* getPhotonHitCollectionElement(const PhotonBox& pBox);
    ObjCollection<PhotonHit>* getPhotonHitCollection(const PhotonBox& pBox);
    int getNbPhotonHits()const
    {
        return count();
    }
    void trim();

    virtual int compareItems(const PhotonHitCollectionElement* t1,const PhotonHitCollectionElement* t2)const;

protected:
    PhotonHitCollectionElement* addElement(PhotonHitCollectionElement* e);
};
