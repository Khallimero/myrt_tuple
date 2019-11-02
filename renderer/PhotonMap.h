#pragma once

#include "PhotonMapElement.h"
#include "PhotonBox.h"
#include "BinaryTree.h"

class PhotonMap:public BinaryTree<PhotonMapElement>
{
public:
    PhotonMap();
    virtual ~PhotonMap();

public:
    virtual int compareItems(const PhotonMapElement* t1,const PhotonMapElement* t2)const;

public:
    void addPhotonHit(int shapeId,const PhotonHit& ph);
    ObjCollection<PhotonHit>* addPhotonBox(int shapeId,const PhotonBox& pBox);
    const ObjCollection<PhotonHit>* getPhotonHitCollection(int shapeId,const PhotonBox& pBox);
    int getNbPhotonHitCollection();

protected:
    PhotonMapElement* insertShapeId(int shapeId);
};
