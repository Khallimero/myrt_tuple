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
        return lckTab[0].trylock();
    }
    void waitLock()
    {
        for(int i=1; i<N; i++)
        {
            lckTab[i].lock();
            lckTab[i-1].unlock();
        }
        lockable->lock();
        locked=true;
        lckTab[N-1].unlock();
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
