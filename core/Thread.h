#pragma once

#include "core.h"

#include <pthread.h>

void* runThread(void*);

class Thread
{
    friend void* runThread(void*);

public:
    Thread(void*(*fct)(void*),void* arg);
    virtual ~Thread();

public:
    void run();
    void wait();
    void kill();
    void setCancelable(bool c=true);

public:
    static void run(void*(*fct)(void*),void* arg,int nb_thread=NB_THREAD);

protected:
    pthread_t pthread;

protected:
    void*(*fct)(void*);
    void* arg;
};
