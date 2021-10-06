#pragma once

#include <stdlib.h>

template <typename T> class _SmartPointer
{
public:
    _SmartPointer(T* t=NULL)
    {
        this->t=t;
    }
    virtual ~_SmartPointer()
    {
    }

public:
    bool operator==(const _SmartPointer<T>& that)const
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
    T* _detach()
    {
        T* tmp=t;
        t=NULL;
        return tmp;
    }

protected:
    T* t;
};

template <typename T> class SmartPointer:public _SmartPointer<T>
{
public:
    SmartPointer(T* t=NULL):_SmartPointer<T>(t)
    {
        this->nb_ref=(t==NULL?NULL:new unsigned int(1));
    }
    SmartPointer(const SmartPointer<T> &that):_SmartPointer<T>(NULL)
    {
        this->nb_ref=NULL;
        *this=that;
    }
    virtual ~SmartPointer()
    {
        _free();
    }

public:
    SmartPointer<T>& operator=(T* t)
    {
        *this=SmartPointer<T>(t);
        return *this;
    }

    SmartPointer<T>&  operator=(const SmartPointer<T> &that)
    {
        _free();

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

        return _SmartPointer<T>::_detach();
    }

protected:
    void _free()
    {
        if(nb_ref!=NULL)
        {
            if(--(*nb_ref)==0)
            {
                delete nb_ref;
                if(this->t!=NULL)
                    delete this->t;
            }
        }
    }

protected:
    unsigned int *nb_ref;
};

template <typename T> class LocalPointer:public _SmartPointer<T>
{
public:
    LocalPointer(T* t=NULL):_SmartPointer<T>(t)
    {
    }
    virtual ~LocalPointer()
    {
        if(this->t!=NULL)
            free(this->t);
    }

public:
    T* detach()
    {
        return _SmartPointer<T>::_detach();
    }
};
