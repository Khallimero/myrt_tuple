#pragma once

#include "Canvas.h"
#include "Color.h"

#include <stdio.h>

class Picture:public Canvas<Color>
{
public:
    Picture(int w,int h);
    Picture(const char* fileName);
    Picture(FILE* f);
    virtual ~Picture();

public:
    void write(const char* fileName,const char* fmt="P6");
    void write(FILE *f,const char* fmt="P6")const;

public:
    Picture* add(const Picture* that,int x=0,int y=0);
    Picture* mul(double f);

protected:
    void buildFromFile(FILE* f);
};
