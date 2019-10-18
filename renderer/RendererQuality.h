#pragma once

class RendererQuality
{
public:
    RendererQuality(int aa=1,int ss=1,int fc=1);
    virtual ~RendererQuality();

public:
    int getAliasing()const
    {
        return aa;
    }
    void setAliasing(int aa)
    {
        this->aa=aa;
    }
    int getShadow()const
    {
        return ss;
    }
    void setShadow(int ss)
    {
        this->ss=ss;
    }
    int getFocus()const
    {
        return fc;
    }
    void setFocus(int fc)
    {
        this->fc=fc;
    }

public:
    static const RendererQuality Default;

protected:
    int aa,ss,fc;
};
