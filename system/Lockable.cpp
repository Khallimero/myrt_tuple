#include "Lockable.h"

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
    return pthread_mutex_lock(&mtx);
}

int Lockable::_trylock()const
{
    return pthread_mutex_trylock(&mtx);
}

int Lockable::_unlock()const
{
    return pthread_mutex_unlock(&mtx);
}
