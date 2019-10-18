#pragma once

#include "IteratorElement.h"

template <typename T> class Iterator
{
public:
    Iterator()
    {
        steps=1;
        reset();
    }
    Iterator(int s,const T& d)
    {
        this->steps=s,this->delta=d;
        reset();
    }
    ~Iterator() {}

public:
    bool hasNext()const volatile
    {
        return (id<steps);
    }

    IteratorElement<T> next()volatile
    {
        return getElement(id++);
    }

    IteratorElement<T> getElement(int i)const volatile
    {
        return IteratorElement<T>(i,(T)(delta*((double)i-(double)(steps-1)/2.0)));
    }

    T maxVal()const
    {
        return (T)(delta*((double)(steps-1)/(double)2.0));
    }

    void reset()volatile
    {
        id=0;
    }

public:
    const T& getDelta()const
    {
        return delta;
    }
    int getSteps()const
    {
        return steps;
    }

protected:
    T delta;
    int steps;
    volatile int id;
};

template <> class Iterator<int>
{
public:
    Iterator(int u=0)
    {
        lwr=0,upr=u,delta=1;
        reset();
    }
    Iterator(int l,int u,int d=1)
    {
        this->lwr=l,this->upr=u,this->delta=d;
        reset();
    }
    ~Iterator() {}

public:
    bool hasNext()const volatile
    {
        return getElement(id).getVal()<=upr;
    }

    IteratorElement<int> next()volatile
    {
        return getElement(id++);
    }

    IteratorElement<int> getElement(int i)const volatile
    {
        return IteratorElement<int>(i,lwr+i*delta);
    }

    int maxVal()const
    {
        return upr;
    }

    void reset()volatile
    {
        id=0;
    }

public:
    const int getDelta()const
    {
        return delta;
    }
    int getSteps()const
    {
        return (1+upr-lwr)/delta;
    }

protected:
    int delta;
    int lwr,upr;
    volatile int id;
};
