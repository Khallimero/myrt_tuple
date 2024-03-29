#pragma once

#include "Lockable.h"

template <int N> class LockQueue
{
public:
    LockQueue(const Lockable* l)
    {
        this->lockable=l;
    }

    virtual ~LockQueue() {}

public:
    const Lockable* tryEnqueueLock()
    {
        if(N==0)
        {
            if(lockable->trylock()==0)
                return lockable;
        }
        else
        {
            for(int i=N; i>0; i--)
                if(lckTab[i-1].trylock()==0)
                    return &lckTab[i-1];
        }

        return NULL;
    }

    void waitLock(const Lockable* l)
    {
        if(N>0)
        {
            for(int i=N; i>0; i--)
            {
                if(&lckTab[i-1]==l)
                {
                    for(int j=i; j<N; j++)
                    {
                        lckTab[j].lock();
                        lckTab[j-1].unlock();
                    }
                    lock();
                    lckTab[N-1].unlock();
                    return;
                }
            }
        }
    }

    void lock()
    {
        lockable->lock();
    }

    void unlock()
    {
        lockable->unlock();
    }

protected:
    const Lockable *lockable;
    const Lockable lckTab[N];
    bool locked;
};
