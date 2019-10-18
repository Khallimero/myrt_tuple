#include "Picture.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Picture::Picture(int w,int h):Map<Color>(w,h) {}

Picture::Picture(const char* fileName):Map<Color>()
{
    FILE* f;
    if((f=fopen(fileName,"r"))!=NULL)
    {
        buildFromFile(f);
        fclose(f);
    }
    else
    {
        fprintf(stderr,"Error reading file %s\n",fileName);
        fflush(stderr);
        exit(1);
    }
}

Picture::Picture(FILE* f):Map<Color>()
{
    buildFromFile(f);
}

Picture::~Picture() {}

Picture* Picture::add(const Picture* that,int x,int y)
{
    for(int i=0; i<MIN(this->height()-y,that->height()); i++)
        for(int j=0; j<MIN(this->width()-x,that->width()); j++)
            (*this)[j+x][i+y]+=(*that)[j][i];
    return this;
}

Picture* Picture::mul(double f)
{
    for(int i=0; i<height(); i++)
        for(int j=0; j<width(); j++)
            (*this)[j][i]*=f;
    return this;
}

void Picture::write(const char* fileName,const char* fmt)
{
    FILE* f;
    if((f=fopen(fileName,"w"))!=NULL)
    {
        write(f,fmt);
        fclose(f);
    }
    else
    {
        fprintf(stderr,"Error writting %s\n",fileName);
        fflush(stderr);
        exit(1);
    }
}

void Picture::write(FILE *f,const char* fmt)const
{
    if(strcmp(fmt,"P3")==0)
    {
        fprintf(f,"P3\n");
    }
    else if(strcmp(fmt,"P6")==0)
    {
        fprintf(f,"P6\n");
    }
    else
    {
        fprintf(stderr,"Invalid format : %s\n",fmt);
        fflush(stderr);
        exit(1);
    }
    fprintf(f,"%d %d\n",width(),height());
    fprintf(f,"%d\n",255);
    for(int i=0; i<height(); i++)
        for(int j=0; j<width(); j++)
            Color((*this)[j][i]).write(f,fmt);
}

void Picture::buildFromFile(FILE* f)
{
    char fmt[2];
    fscanf(f,"%s",fmt);
    if(strcmp(fmt,"P3")==0||strcmp(fmt,"P6")==0)
    {
        int d;
        fscanf(f,"%d %d %d\n",&(Map<Color>::w),&(Map<Color>::h),&d);
        Map<Color>::alloc();
        for(int y=0; y<height(); y++)
        {
            for(int x=0; x<width(); x++)
            {
                (*this)[x][y]=Color(f,fmt,d);
            }
        }
    }
    else
    {
        fprintf(stderr,"Error reading file\n");
        fflush(stderr);
        exit(1);
    }
}
