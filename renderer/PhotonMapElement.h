#pragma once

#include "PhotonHitCollection.h"
#include "Shape.h"

class PhotonMapElement
{
public:
    PhotonMapElement(int shapeId);
    virtual ~PhotonMapElement();

public:
    virtual int compareTo(const PhotonMapElement* that)const;

public:
    int getShapeId()const
    {
        return shapeId;
    }
    PhotonHitCollection& getCollection()
    {
        return c;
    }

protected:
    int shapeId;
    PhotonHitCollection c;
};
