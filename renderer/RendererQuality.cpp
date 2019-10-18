#include "RendererQuality.h"

RendererQuality::RendererQuality(int aa,int ss,int fc)
{
    this->aa=aa,this->ss=ss,this->fc=fc;
}

RendererQuality::~RendererQuality() {}

const RendererQuality RendererQuality::Default=RendererQuality();
