#pragma once

#include "system.h"
#include "Lockable.h"
#include "AutoLock.h"

#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

template <typename T> class ICollection:public Lockable
{
protected:
    ICollection():Lockable()
    {
        this->nb=0;
        this->nb_alloc=0;
        this->tab=NULL;
    }

    virtual ~ICollection() {}

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

    virtual void del(const T& t)=0;

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
    virtual void _free()=0;
    virtual void _resize(int s,bool f=true)=0;

protected:
    volatile int nb;
    mutable int nb_alloc;
    T* tab;
};

template <typename T> class Collection:public ICollection<T>
{
public:
    Collection() {}

    Collection(const Collection& that)
    {
        *this=that;
    }

    Collection& operator=(const Collection& that)
    {
        _free();
        this->_reserve(that.nb,false);
        memcpy(this->tab,that.tab,that.nb*sizeof(T));
        this->nb=that.nb;

        return *this;
    }

    virtual ~Collection()
    {
        _free();
    }

public:
    virtual void del(const T& t)
    {
        AutoLock autolock(this);
        for(int i=0; i<this->nb; i++)
        {
            if(this->tab[i]==t)
            {
                memmove(&this->tab[i],&this->tab[i+1],((--this->nb)-i)*sizeof(T));
                return;
            }
        }
    }

private:
    virtual void _free()
    {
        if(this->tab)
        {
            free(this->tab);
            this->tab=NULL;
        }
        this->nb_alloc=0;
    }

    virtual void _resize(int s,bool f=true)
    {
        this->nb=MIN(this->nb,s);
        this->tab=(T*)realloc(this->tab,s*sizeof(T));
        if(f&&s>this->nb)memset(&this->tab[this->nb],0,(s-this->nb)*sizeof(T));
        this->nb_alloc=s;
    }
};

template <typename T> class ObjCollection:public ICollection<T>
{
public:
    ObjCollection(int n=0)
    {
        if(n>0)_resize(n);
    }
    ObjCollection(const ObjCollection& that)
    {
        *this=that;
    }

    ObjCollection& operator=(const ObjCollection& that)
    {
        _free();
        this->_reserve(that.nb,false);
        for(int i=0; i<this->nb; i++)
            this->tab[i]=that.tab[i];
        this->nb=that.nb;

        return *this;
    }

    virtual ~ObjCollection()
    {
        _free();
    }

public:
    virtual void del(const T& t)
    {
        AutoLock autolock(this);
        for(int i=0; i<this->nb; i++)
        {
            if(this->tab[i]==t)
            {
                this->nb--;
                for(int j=i; j<this->nb; j++)
                    this->tab[j]=this->tab[j+1];
                return;
            }
        }
    }

private:
    virtual void _free()
    {
        if(this->tab)
        {
            delete[] this->tab;
            this->tab=NULL;
        }
        this->nb_alloc=0;
    }

    virtual void _resize(int s,bool f=true)
    {
        this->nb=MIN(this->nb,s);
        T* t=new T[s];
        if(this->tab)
        {
            for(int i=0; i<this->nb; i++)
                t[i]=this->tab[i];
            delete[] this->tab;
        }
        this->tab=t;
        this->nb_alloc=s;
    }
};

template <typename T> class CollectionUnion
{
public:
    CollectionUnion(int n,...)
    {
        this->tab=NULL;
        _resize(n);
        va_list vl;
        va_start(vl,n);
        for(int i=0; i<=n; i++)
            this->tab[i]=va_arg(vl,ICollection<T>*);
        va_end(vl);
    }

    CollectionUnion(const CollectionUnion& that)
    {
        this->tab=NULL;
        *this=that;
    }

    CollectionUnion& operator=(const CollectionUnion& that)
    {
        _resize(that.nb);
        memcpy(this->tab,that.tab,this->nb*sizeof(ICollection<T>*));
        return *this;
    }

    virtual ~CollectionUnion()
    {
        _free();
    }

private:
    virtual void _free()
    {
        if(this->tab)
        {
            free(this->tab);
            this->tab=NULL;
        }
        this->nb=0;
    }

    virtual void _resize(int s)
    {
        this->tab=(ICollection<T>**)realloc(this->tab,s*sizeof(ICollection<T>*));
        this->nb=s;
    }

public:
    int count()const
    {
        int cnt=0;
        for(int i=0; i<this->nb; i++)
            cnt+=this->tab[i]->count();
        return cnt;
    }

    const int _count()const
    {
        int cnt=0;
        for(int i=0; i<this->nb; i++)
            cnt+=this->tab[i]->_count();
        return cnt;
    }

    const T& get(int i)const
    {
        for(int j=0; j<this->nb; j++)
        {
            if(i<this->tab[j]->_count())
                return this->tab[j]->get(i);
            i-=this->tab[j]->_count();
        }
        return NULL;
    }

    const T& operator[](int i)const
    {
        return get(i);
    }

protected:
    ICollection<T>** tab;
    int nb;
};
