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
    int tryEnqueueLock()
    {
        if(N==0)return lockable->trylock();
        return lckTab[0].trylock();
    }
    void waitLock()
    {
        if(N>0)
        {
            for(int i=1; i<N; i++)
            {
                lckTab[i].lock();
                lckTab[i-1].unlock();
            }
            lockable->lock();
            lckTab[N-1].unlock();
        }
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

protected:
    const Lockable *lockable;
    const Lockable lckTab[N];
    bool locked;
};
