#include "Fog.h"

Fog::Fog()
{
    this->col=Color::White;
    this->dst=-1.0;
    this->pw=1.0;
}

Fog::Fog(const Color& c,double d,double p)
{
    this->col=c,this->dst=d,this->pw=p;
}

Fog::~Fog() {}
