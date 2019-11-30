#pragma once

#include <pthread.h>

class Lockable
{
public:
    Lockable(bool s=true);
    virtual ~Lockable();

public:
    void setThreadSafe(bool s);
    bool isThreadSafe()const
    {
        return tsafe;
    }

public:
    int lock()const;
    int trylock()const;
    int unlock()const;

protected:
    int _lock()const;
    int _trylock()const;
    int _unlock()const;

protected:
    bool tsafe;
    mutable pthread_mutex_t mtx;
};
