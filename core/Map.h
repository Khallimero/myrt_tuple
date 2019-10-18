#pragma once

#include "Point.h"

#include <stdlib.h>

template <typename T> T bilinearFilter(T** tab,int w,int h,double x0,double y0);

template <typename T> class Map
{
public:
    Map()
    {
        this->w=0,this->h=0;
        tab=NULL;
    }
    Map(int w,int h)
    {
        this->w=w,this->h=h;
        alloc();
    }
    virtual ~Map()
    {
        if(tab!=NULL)
        {
            for(int i=0; i<w; i++)
                free(tab[i]);
            free(tab);
            tab=NULL;
        }
    }

public:
    virtual T* operator[](int i)const
    {
        return tab[i];
    }

    virtual T getElem(const Point& p)const
    {
        double x0=p.getX()*(double)width();
        double y0=p.getY()*(double)height();
        return bilinearFilter<T>(tab,width(),height(),x0,y0);
    }

public:
    virtual int width()const
    {
        return w;
    }
    virtual int height()const
    {
        return h;
    }
    virtual T** getTab()const
    {
        return tab;
    }

protected:
    virtual void alloc()
    {
        tab=(T**)malloc(w*sizeof(T*));
        for(int i=0; i<w; i++)
            tab[i]=(T*)malloc(h*sizeof(T));
    }

protected:
    T** tab;
    int w,h;
};
