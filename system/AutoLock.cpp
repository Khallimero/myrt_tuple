#include "AutoLock.h"

AutoLock::AutoLock(const Lockable* l,bool wait)
{
    this->lockable=l;
    this->locked=false;
    this->lock(wait);
}

AutoLock::~AutoLock()
{
    this->unlock();
}

int AutoLock::lock(bool wait)
{
    if((wait?this->lockable->lock():this->lockable->trylock())==0)
    {
        this->locked=true;
        return true;
    }
    return false;
}

void AutoLock::unlock()
{
    if(this->locked)
    {
        this->locked=false;
        this->lockable->unlock();
    }
}
