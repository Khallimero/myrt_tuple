#include "Thread.h"

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

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
    if(_nb_thread<=0)
    {
#ifdef NB_THREAD
        _nb_thread=NB_THREAD;
#else
        cpu_set_t set;
        if(pthread_getaffinity_np(pthread_self(),sizeof(set),&set)!=0)
        {
            fprintf(stderr,"CPU count error.\n");
            fflush(stderr);
            exit(1);
        }
        _nb_thread=CPU_COUNT(&set);
#endif
        fprintf(stdout,"CPU count : %d\n",_nb_thread);
        fflush(stdout);
#ifdef OPENCL_QUEUE
        //_nb_thread+=OPENCL_QUEUE;
        //fprintf(stdout,"Thread count : %d\n",_nb_thread);
        //fflush(stdout);
#endif
    }

    return _nb_thread;
}

int Thread::threadId()
{
    return (int)gettid();
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
