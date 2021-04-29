#pragma once

#include <stdlib.h>

template <typename T> class TSmartPointer
{
public:
    TSmartPointer(T* t=NULL)
    {
        this->t=t;
        this->nb_ref=(t==NULL?NULL:new unsigned int(1));
    }
    TSmartPointer(const TSmartPointer<T> &that)
    {
        this->t=NULL;
        this->nb_ref=NULL;
        *this=that;
    }
    virtual ~TSmartPointer()
    {
        if(nb_ref!=NULL)
        {
            if(--(*nb_ref)==0)
            {
                delete nb_ref;
                if(t!=NULL)_free();
            }
        }
    }

protected:
    virtual void _free() {}

public:
    TSmartPointer<T>& operator=(const TSmartPointer<T> &that)
    {
        detach();

        this->t=that.t;
        this->nb_ref=that.nb_ref;

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

    bool operator==(const TSmartPointer<T>& that)const
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
};

template <typename T> class SmartPointer:public TSmartPointer<T>
{
public:
    SmartPointer(T* t=NULL):TSmartPointer<T>(t)
    {
    }

protected:
    virtual void _free()
    {
        delete this->t;
    }
};

template <typename T> class SmartTabPointer:public TSmartPointer<T>
{
public:
    SmartTabPointer(T* t=NULL):TSmartPointer<T>(t)
    {
    }

protected:
    virtual void _free()
    {
        free(this->t);
    }
};
