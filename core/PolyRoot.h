#pragma once

#include "Collection.h"

class PolyRoot:protected Collection<double>
{
public:
    PolyRoot(bool collect=true);
    virtual ~PolyRoot();

public:
    int getNbRoots()const
    {
        return _count();
    }

    double operator[](int i)const
    {
        return get(i);
    }

public:
    void addRoot(double r);

    double getFirstRoot()const
    {
        return firstRoot;
    }

protected:
    double firstRoot;
    bool collect;
};
