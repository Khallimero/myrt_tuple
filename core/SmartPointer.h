#pragma once

#include <stdlib.h>

template <typename T> class SmartPointer
{
public:
    SmartPointer(T* t=NULL,bool tab=false)
    {
        this->t=t;
        this->nb_ref=(t==NULL?NULL:new unsigned int(1));
        this->tab=tab;
    }
    SmartPointer(const SmartPointer<T> &that)
    {
        this->t=NULL;
        this->nb_ref=NULL;
        *this=that;
    }
    ~SmartPointer()
    {
        if(nb_ref!=NULL)
        {
            if(--(*nb_ref)==0)
            {
                delete nb_ref;
                if(t!=NULL)
                {
                    if(tab)delete[] t;
                    else delete t;
                }
            }
        }
    }

public:
    SmartPointer<T>& operator=(const SmartPointer<T> &that)
    {
        detach();

        this->t=that.t;
        this->nb_ref=that.nb_ref;
        this->tab=that.tab;

        if(nb_ref!=NULL)
            (*nb_ref)++;

        return *this;
    }

    T* detach()
    {
        if(nb_ref!=NULL)
        {
            if(--(*nb_ref)==0)
                delete nb_ref;
            nb_ref=NULL;
        }
        return t;
    }

    bool operator==(const SmartPointer<T>& that)const
    {
        return this->getPointer()==that.getPointer();
    }

public:
    operator T*()
    {
        return t;
    }
    operator T&()
    {
        return *t;
    }
    T* operator->()
    {
        return t;
    }
    T* getPointer()
    {
        return t;
    }
    const T* operator->()const
    {
        return t;
    }
    const T* getPointer()const
    {
        return t;
    }
    T& operator*()
    {
        return *t;
    }

protected:
    T* t;
    unsigned int* nb_ref;
    bool tab;
};
