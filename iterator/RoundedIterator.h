#pragma once

#include "NestedIterator.h"

class RoundedIterator:public NestedIterator<double,2>
{
public:
    RoundedIterator();
    RoundedIterator(const Iterator<double>& it);
    virtual ~RoundedIterator();

public:
    virtual bool next();

protected:
    double getRadius()const;

protected:
    double r;
};
