#pragma once

template <typename T> class IteratorElement
{
public:
    IteratorElement()
    {
        id=-1;
    }
    IteratorElement(int i,const T& v)
    {
        this->id=i,this->val=v;
    }
    virtual ~IteratorElement() {}

public:
    int getId()const
    {
        return id;
    }
    const T& getVal()const
    {
        return val;
    }
    operator const T&()const
    {
        return val;
    }

protected:
    int id;
    T val;
};
