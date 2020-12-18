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
        if((wait?this->lockable->lock():this->lockable->trylock())==0)
        {
            this->locked=true;
        }
    }
    else this->locked=false;
    return this->locked;
}

void AutoLock::unlock()
{
    if(this->locked)
    {
        this->locked=false;
        lockable->unlock();
    }
}
