#pragma once

#include "Iterator.h"
#include "IteratorElement.h"
#include "Collection.h"
#include "Tuple.h"

class INestedIterator
{
public:
    INestedIterator();
    virtual ~INestedIterator();

public:
    virtual void reset(int n=0,bool itLast=false)=0;

    virtual bool hasNext()const;
    virtual bool next();

protected:
    virtual bool _hasNext(int i)const=0;
    virtual int _getSteps(int i)const=0;

    virtual bool _next();

public:
    virtual int getActualSteps()const
    {
        return nb;
    }
    int size()const
    {
        return nbIt;
    }

    virtual int getMaxSteps()const;

protected:
    int nbIt,nb;
};

template <typename T> class ITNestedIterator:public INestedIterator
{
public:
    ITNestedIterator()
    {
        reset();
    }
    virtual ~ITNestedIterator() {}

public:
    const Iterator<T>& getIterator(int i)const
    {
        return const_cast<ITNestedIterator*>(this)->getIterators()[i];
    }
    const IteratorElement<T>& getElement(int i)const
    {
        return const_cast<ITNestedIterator*>(this)->getIteratorElements()[i];
    }
    const IteratorElement<T>& operator[](int i)const
    {
        return getElement(i);
    }

    virtual void add(const Iterator<T>& that,bool rst=true)=0;

    virtual void reset(int n=0,bool itLast=false)
    {
        for(int i=n; i<this->nbIt; i++)
        {
            Iterator<T> *iter=getIterators();
            IteratorElement<T> *elem=const_cast<IteratorElement<T>*>(getIteratorElements());

            if(i!=n||!itLast)
                iter[i].reset();
            if(i!=this->nbIt-1||itLast)
                elem[i]=iter[i].next();
        }
        if(!itLast)this->nb=0;
    }

protected:
    virtual Iterator<T>* getIterators()=0;
    virtual IteratorElement<T>* getIteratorElements()=0;

    virtual bool _hasNext(int i)const
    {
        return getIterator(i).hasNext();
    }

    virtual int _getSteps(int i)const
    {
        return getIterator(i).getSteps();
    }

    void _init(const Iterator<T>& that,int n=1)
    {
        nbIt=0,nb=0;
        while(n>0)
            add(that,(--n)==0);
    }
};

template <typename T,int N> class NestedIterator:public ITNestedIterator<T>
{
public:
    NestedIterator() {}
    NestedIterator(const Iterator<T>& that,int n=1)
    {
        this->_init(that,n);
    }
    virtual ~NestedIterator() {}

public:
    void add(const Iterator<T>& that,bool rst=true)
    {
        if(this->nbIt<N)it[this->nbIt++]=that;
        if(rst)this->reset();
    }

    Tuple<T,N> getTuple()const
    {
        struct tuple_unary_idx_op:public unary_idx_op<T,N>
        {
            tuple_unary_idx_op(const ITNestedIterator<T>* _it):it(_it) {}
            virtual T operator()(const tuple_idx<N>& idx)const
            {
                return it->getElement(idx).getVal();
            }
            const ITNestedIterator<T> *it;
        } _tuple_unary_idx_op(this);
        return Tuple<T,N>(_tuple_unary_idx_op);
    }

protected:
    virtual Iterator<T>* getIterators()
    {
        return it;
    }
    virtual IteratorElement<T>* getIteratorElements()
    {
        return ie;
    }

protected:
    Iterator<T> it[N];
    IteratorElement<T> ie[N];
};

template <typename T> class NestedIterator<T,0>:public ITNestedIterator<T>
{
public:
    NestedIterator() {}
    NestedIterator(const Iterator<T>& that,int n=1)
    {
        it.reserve(n),ie.reserve(n);
        _init(that,n);
    }
    virtual ~NestedIterator() {}

public:
    void trim()
    {
        it.trim(),ie.trim();
    }

public:
    void add(const Iterator<T>& that,bool rst=true)
    {
        it._add(that),ie._add(IteratorElement<T>());
        this->nbIt++;
        if(rst)this->reset();
    }

protected:
    virtual Iterator<T>* getIterators()
    {
        return it.getTab();
    }
    virtual IteratorElement<T>* getIteratorElements()
    {
        return ie.getTab();
    }

protected:
    Collection< Iterator<T> > it;
    Collection< IteratorElement<T> > ie;
};
