#include "Lockable.h"

#define MTX_LOCK(mtx) pthread_mutex_lock(&mtx)
#define MTX_TRYLOCK(mtx) pthread_mutex_trylock(&mtx)
#define MTX_UNLOCK(mtx) pthread_mutex_unlock(&mtx)

Lockable::Lockable(bool s)
{
    pthread_mutex_init(&mtx,NULL);
    setThreadSafe(s);
}

Lockable::~Lockable()
{
    pthread_mutex_destroy(&mtx);
}

void Lockable::setThreadSafe(bool s)
{
    _lock();
    tsafe=s;
    _unlock();
}

int Lockable::lock()const
{
    return tsafe?_lock():0;
}

int Lockable::trylock()const
{
    return tsafe?_trylock():0;
}

int Lockable::unlock()const
{
    return tsafe?_unlock():0;
}


int Lockable::_lock()const
{
    return MTX_LOCK(mtx);
}

int Lockable::_trylock()const
{
    return MTX_TRYLOCK(mtx);
}

int Lockable::_unlock()const
{
    return MTX_UNLOCK(mtx);
}
