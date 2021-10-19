#pragma once

#include "Iterator.h"
#include "NestedIterator.h"

template <typename T> class Enumeration
{
public:
    Enumeration()
    {
        this->nb=1,this->elem=T();
    }
    Enumeration(int n,T e)
    {
        this->nb=n,this->elem=e;
    }
    virtual ~Enumeration() {}

public:
    Iterator<double> getIterator()const
    {
        return Iterator<double>(nb,1);
    }

    NestedIterator<double,2> getNestedIterator()const
    {
        return NestedIterator<double,2>(getIterator(),2);
    }

    const T getElem(const IteratorElement<double>& i)const
    {
        return elem*i.getVal();
    }

public:
    const T& getElem()const
    {
        return elem;
    }
    int getNb()const
    {
        return nb;
    }

protected:
    int nb;
    T elem;
};

