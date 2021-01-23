#pragma once

#include "Treble.h"

#include <stdio.h>

#define NORM_VALUE 255

class Color:public Treble<double>
{
public:
    Color();
    Color(const char* hex);
    Color(const unsigned int v);
    Color(const Tuple<double,TREBLE_SIZE> &that);
    Color(double r,double g,double b);
    Color(const double* tab);
    virtual ~Color();

public:
    Color(FILE* f,const char* fmt="P6",int norm_value=NORM_VALUE);
    virtual void write(FILE *f,const char* fmt="P6",int norm_value=NORM_VALUE)const;

public:
    Color norm()const;
    Color negative()const;
    Color beer(double bCoeff)const;

public:
    double red()const
    {
        return this->vTab[ITreble::first];
    }
    double green()const
    {
        return this->vTab[ITreble::second];
    }
    double blue()const
    {
        return this->vTab[ITreble::third];
    }
    double grey()const;

    void setRed(double red)
    {
        this->vTab[ITreble::first]=red;
    }
    void setGreen(double green)
    {
        this->vTab[ITreble::second]=green;
    }
    void setBlue(double blue)
    {
        this->vTab[ITreble::third]=blue;
    }

private:
    static double denormVal(int x,int v=NORM_VALUE)
    {
        return ((double)x)/((double)v);
    }
    static int normVal(double x,int v=NORM_VALUE)
    {
        return (int)(Color::_norm(x)*((double)v));
    }
    static double _norm(double x)
    {
        return MIN(MAX(x,0.0),1.0);
    }

public:
    static const Color White;
    static const Color Black;
};
