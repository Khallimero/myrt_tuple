#include "PhotonMapElement.h"

PhotonMapElement::PhotonMapElement(int shapeId)
{
    this->shapeId=shapeId;
}

PhotonMapElement::~PhotonMapElement() {}

int PhotonMapElement::compareTo(const PhotonMapElement* that)const
{
    return this->shapeId-that->shapeId;
}
