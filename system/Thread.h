#pragma once

#include "system.h"

#include <pthread.h>

#define NB_THREAD 8

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
    static int nbThread();
    static void run(void*(*fct)(void*),void* arg,int nb_thread=-1);

protected:
    pthread_t pthread;
    static int _nb_thread;

protected:
    void*(*fct)(void*);
    void* arg;
};
