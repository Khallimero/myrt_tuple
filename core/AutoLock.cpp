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
    if(this->lockable!=NULL)
    {
        if(!this->locked)
            this->locked=((wait?this->lockable->lock():this->lockable->trylock())==0);
    }
    else this->locked=false;
    return this->locked;
}

void AutoLock::unlock()
{
    if(this->locked)
    {
        lockable->unlock();
        this->locked=false;
    }
}
