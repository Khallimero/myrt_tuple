#pragma once

#include "core.h"
#include "Lockable.h"
#include "AutoLock.h"

#include <stdlib.h>
#include <string.h>

template <typename T> class Collection:public Lockable
{
public:
    Collection(bool obj=false):Lockable()
    {
        this->nb=0;
        this->nb_alloc=0;
        tab=NULL;
        this->obj=obj;
    }
    Collection(const Collection& that)
    {
        this->tab=NULL;
        *this=that;
    }

    Collection& operator=(const Collection& that)
    {
        _free();
        this->nb_alloc=0;
        this->tab=NULL;
        this->obj=that.obj;
        _reserve(that.nb,false);
        if(this->obj)
        {
            for(int i=0; i<this->nb; i++)
                this->tab[i]=that.tab[i];
        }
        else
        {
            memcpy(this->tab,that.tab,that.nb*sizeof(T));
        }
        this->nb=that.nb;

        return *this;
    }

    virtual ~Collection()
    {
        _free();
    }

public:
    void add(const T& t)
    {
        AutoLock autolock(this);
        _add(t);
    }

    void _add(const T& t)
    {
        if(nb>=nb_alloc)
            _reserve(nb_alloc+1+(nb_alloc>>2));
        tab[nb++]=t;
    }

    void reserve(int s)
    {
        AutoLock autolock(this);
        _reserve(s);
    }

    void _reserve(int s,bool f=true)
    {
        if(s>nb_alloc)
            _resize(s,f);
    }

    int indexOf(const T& t)const
    {
        AutoLock autolock(this);
        for(int i=0; i<nb; i++)
            if(tab[i]==t)
                return i;
        return -1;
    }

    void replace(const T& t1,const T& t2)
    {
        AutoLock autolock(this);
        for(int i=0; i<nb; i++)
        {
            if(tab[i]==t1)
            {
                tab[i]=t2;
                return;
            }
        }
    }

    void del(const T& t)
    {
        AutoLock autolock(this);
        for(int i=0; i<nb; i++)
        {
            if(tab[i]==t)
            {
                if(obj)
                {
                    nb--;
                    for(int j=i; j<nb; j++)
                        tab[j]=tab[j+1];
                }
                else
                {
                    memmove(&tab[i],&tab[i+1],((--nb)-i)*sizeof(T));
                }
                return;
            }
        }
    }

    bool delLast(T& t)
    {
        AutoLock autolock(this);
        if(nb>0)
        {
            t=tab[--nb];
            return true;
        }
        return false;
    }

    void trim()
    {
        AutoLock autolock(this);
        if(nb!=nb_alloc)
            _resize(nb);
    }

    void clear()
    {
        AutoLock autolock(this);
        _clear();
    }

    void _clear()
    {
        nb=0;
    }

    const T& get(int i)const
    {
        return tab[i];
    }

    const T& operator[](int i)const
    {
        return get(i);
    }

    T& getRef(int i)
    {
        AutoLock autolock(this);
        return _getRef(i);
    }

    T& _getRef(int i)
    {
        _reserve(i+1);
        this->nb=MAX(this->nb,i+1);
        return tab[i];
    }

    void set(const T& t,int i)
    {
        AutoLock autolock(this);
        _set(t,i);
    }

    void _set(const T& t,int i)
    {
        _getRef(i)=t;
    }

public:
    int count()const
    {
        AutoLock autolock(this);
        return _count();
    }
    const volatile int& _count()const
    {
        return nb;
    }
    T* getTab()
    {
        return tab;
    }

private:
    void _free()
    {
        if(tab)
        {
            if(obj)
                delete[] tab;
            else
                free(tab);
        }
    }

    void _resize(int s,bool f=true)
    {
        nb=MIN(nb,s);
        if(obj)
        {
            T* t=new T[s];
            if(tab)
            {
                for(int i=0; i<nb; i++)
                    t[i]=tab[i];
                delete[] tab;
            }
            tab=t;
        }
        else
        {
            tab=(T*)realloc(tab,s*sizeof(T));
            if(f&&s>nb)memset(&tab[nb],0,(s-nb)*sizeof(T));
        }
        nb_alloc=s;
    }

protected:
    volatile int nb;
    mutable int nb_alloc;
    T* tab;
    bool obj;
};
