#pragma once

#include "Lockable.h"

template <int N> class ConcurrentLock
{
public:
    ConcurrentLock() {}
    virtual ~ConcurrentLock() {}

public:
    const Lockable* tryLock()
    {
        for(int i=0; i<N; i++)
            if(lckTab[i].trylock()==0)
                return &lckTab[i];
        return NULL;
    }

protected:
    const Lockable lckTab[N];
};
