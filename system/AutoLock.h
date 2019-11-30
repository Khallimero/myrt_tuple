#pragma once

#include "Lockable.h"

class AutoLock
{
public:
    AutoLock(const Lockable* l,bool wait=true);
    virtual ~AutoLock();

public:
    const Lockable* getLockable()const
    {
        return lockable;
    }
    bool isLocked()const
    {
        return locked;
    }
    int lock(bool wait=true);
    void unlock();

protected:
    const Lockable* lockable;
    bool locked;
};
