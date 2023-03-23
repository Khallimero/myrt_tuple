#pragma once

#include "Lockable.h"

template <int N> class LockQueue
{
public:
    LockQueue(const Lockable* l)
    {
        this->lockable=l;
        this->locked=false;
    }

    virtual ~LockQueue()
    {
        this->unlock();
    }

public:
    const Lockable* tryEnqueueLock()
    {
        if(N==0) return lockable->trylock()==0?lockable:NULL;
        for(int i=0; i<N; i++)
        {
            if(lckTab[i].trylock()==0)
            {
                if(i>0)lckTab[i-1].unlock();
            }
            else return i>0?&lckTab[i-1]:NULL;
        }
        return &lckTab[N-1];
    }

    void waitLock(const Lockable* l)
    {
        if(N>0)
        {
            for(int i=0; i<N; i++)
            {
                if(&lckTab[i]==l)
                {
                    for(int j=i+1; j<N; j++)
                    {
                        lckTab[j].lock();
                        lckTab[j-1].unlock();
                    }
                    lockable->lock();
                    lckTab[N-1].unlock();
                    break;
                }
            }
        }
        locked=true;
    }

    void lock()
    {
        lockable->lock();
        locked=true;
    }

    void unlock()
    {
        if(locked)
        {
            locked=false;
            lockable->unlock();
        }
    }

    void abort()
    {
        if(N==0)lockable->unlock();
        else lckTab[0].unlock();
    }

protected:
    const Lockable *lockable;
    const Lockable lckTab[N];
    bool locked;
};
