#include "Thread.h"

#include <stdlib.h>
#include <stdio.h>

int Thread::_nb_thread=-1;

void* runThread(void* th)
{
    Thread* t=(Thread*)th;
    t->fct(t->arg);
    pthread_exit(t);
    return NULL;
};

Thread::Thread(void*(*fct)(void*),void* arg)
{
    this->fct=fct;
    this->arg=arg;
}

Thread::~Thread()
{
}

void Thread::run()
{
    pthread_attr_t t_attr;
    pthread_attr_init(&t_attr);
    pthread_attr_setdetachstate(&t_attr,PTHREAD_CREATE_JOINABLE);

    if(pthread_create(&pthread,&t_attr,runThread,this)!=0)
    {
        fprintf(stderr,"Thread creation error.\n");
        fflush(stderr);
        exit(1);
    }

    pthread_attr_destroy(&t_attr);
}

void Thread::wait()
{
    if(pthread_join(pthread,NULL)!=0)
    {
        fprintf(stderr,"Thread join error.\n");
        fflush(stderr);
    }
}

void Thread::kill()
{
    if(pthread_cancel(pthread)!=0)
    {
        fprintf(stderr,"Thread cancel error.\n");
        fflush(stderr);
    }
}

int Thread::nbThread()
{
    if(_nb_thread<0)
    {
        cpu_set_t set;
        _nb_thread=pthread_getaffinity_np(pthread_self(),sizeof(set),&set)==0?CPU_COUNT(&set):NB_THREAD;
    }
    return _nb_thread;
}

void Thread::run(void*(*fct)(void*),void* arg,int nb_thread)
{
    if(nb_thread==-1)
    {
        nb_thread=Thread::nbThread();
    }

    if(nb_thread<=1)
    {
        fct(arg);
        return;
    }

    Thread **thTab=(Thread**)malloc(nb_thread*sizeof(Thread*));
    if(thTab==NULL)
    {
        fprintf(stderr,"Thread allocation error.\n");
        fflush(stderr);
        exit(1);
    }

    for(int t=0; t<nb_thread; t++)
    {
        thTab[t]=new Thread(fct,arg);
        thTab[t]->run();
    }

    for(int t=0; t<nb_thread; t++)
    {
        thTab[t]->wait();
        delete thTab[t];
    }

    free(thTab);
}

void Thread::setCancelable(bool c)
{
    if(pthread_setcancelstate(c?PTHREAD_CANCEL_ENABLE:PTHREAD_CANCEL_DISABLE,NULL)!=0)
    {
        fprintf(stderr,"Thread cancel state error.\n");
        fflush(stderr);
    }
}
