#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <sys/resource.h>
#include <sys/time.h>

#define ALARM_TIMER (1)

static unsigned int rseed=0;

static void __attribute__((constructor)) seed()
{
    //rseed=(unsigned int)time(NULL);
    //printf("Seeding %u\n",rseed);
    //srand(rseed);
}

static void __attribute__((destructor)) get_mem_usage()
{
    struct rusage usage;
    if(getrusage(RUSAGE_SELF,&usage)==0)
    {
        fprintf(stdout,"User CPU time used: %ld.%06ld sec\n",usage.ru_utime.tv_sec,usage.ru_utime.tv_usec);
        fprintf(stdout,"System CPU time used: %ld.%06ld sec\n",usage.ru_stime.tv_sec,usage.ru_stime.tv_usec);
        fprintf(stdout,"Maximum resident set size: %ld kB\n",usage.ru_maxrss);
        //fprintf(stdout,"Integral shared memory size: %ld kB\n",usage.ru_ixrss);
        //fprintf(stdout,"Integral unshared data size: %ld kB\n",usage.ru_idrss);
        //fprintf(stdout,"Integral unshared stack size: %ld kB\n",usage.ru_isrss);
        fprintf(stdout,"Page reclaims (soft page faults): %ld\n",usage.ru_minflt);
        fprintf(stdout,"Page faults (hard page faults): %ld\n",usage.ru_majflt);
        //fprintf(stdout,"Swaps: %ld\n",usage.ru_nswap);
        fprintf(stdout,"Block input operations: %ld\n",usage.ru_inblock);
        fprintf(stdout,"Block output operations: %ld\n",usage.ru_oublock);
        //fprintf(stdout,"IPC messages sent: %ld\n",usage.ru_msgsnd);
        //fprintf(stdout,"IPC messages received: %ld\n",usage.ru_msgrcv);
        //fprintf(stdout,"Signals received: %ld\n",usage.ru_nsignals);
        fprintf(stdout,"Voluntary context switches: %ld\n",usage.ru_nvcsw);
        fprintf(stdout,"Involuntary context switches: %ld\n",usage.ru_nivcsw);
        fflush(stdout);
    }

    FILE *f=NULL;
    char s[BUFSIZ];
    sprintf(s,"/proc/%d/status",getpid());
    if((f=fopen(s,"r"))!=NULL)
    {
        while(fgets(s,sizeof(s),f))
            if(strncmp(s,"VmPeak:",7)==0||strncmp(s,"VmSize:",7)==0)
                fprintf(stdout,"%s",s);
        fflush(stdout);
        fclose(f);
    }
}

#include "Scene.h"
#include "Camera.h"
#include "Renderer.h"
#include "AutoLock.h"

#define SCENE
#include "myrt.h"

void setAlarm(int timer=ALARM_TIMER);

void alarmHandler(int sig)
{
    AutoLock autolock(&Renderer::Monitored,false);
    if(autolock.isLocked())
    {
        switch(sig)
        {
        case SIGALRM:
            for(int i=0; i<Renderer::Monitored._count(); i++)
            {
                AutoLock autolock(Renderer::Monitored[i],false);
                const Renderer* rnd=(const Renderer*)autolock.getLockable();
                if(rnd!=NULL&&autolock.isLocked())
                {
                    switch(rnd->getRenderingProgress())
                    {
                    case Init:
                        break;
                    case Photon:
                        fprintf(stdout,"[%d] Computing photon map (%d/%d)\n",i,rnd->getProgressCount(),rnd->getPhotonIteratorSteps());
                        break;
                    case Rendering:
                        if(rnd->getPicture()!=NULL)
                        {
                            time_t span=time(NULL)-rnd->getStartTime();
                            if(span<2)break;
                            int prg=rnd->getRndProgressCount();
                            int size=rnd->getPicture()->width()*rnd->getPicture()->height();
                            double pct=(double)prg/size;
                            double pps=(double)prg/span;
                            double spp=span/(double)prg;
                            time_t eta=(time_t)(prg>0?rnd->getStartTime()+span/pct:0L);
                            fprintf(stdout,"[%d] Rendering %d/%d (%.2f%%) %d%s ETA %s",
                                    i,prg,size,(float)pct*100.,prg>0?(int)MAX(pps,spp):0,pps>spp||prg==0?"pps":"spp",prg>0?ctime(&eta):"Unknown\n");
                        }
                        else
                            fprintf(stdout,"[%d] Rendering pixel %d\n",i,rnd->getRndProgressCount());
                        break;
                    case Complete:
                        break;
                    }
                    fflush(stdout);
                }
            }

            setAlarm();
        default:
            break;
        }
    }
}
void setHandler(int signum,void (*hdl)(int))
{
    struct sigaction action;
    sigemptyset(&action.sa_mask);
    action.sa_handler=hdl;
    action.sa_flags=0;
    sigaction(signum,&action,NULL);
}

void setAlarm(int timer)
{
    setHandler(SIGALRM,alarmHandler);
    alarm(timer);
}

#ifndef RSEED
#define RSEED (unsigned int)(time(NULL))
#endif

int main(void)
{
    rseed=RSEED;
    printf("Seeding %u\n",rseed);
    srand(rseed);

    setAlarm();

    Scene world;
    world.setSeed(rseed);
    buildScene(world);

    Camera cam;
    setCamera(cam);

    RendererQuality rendererQuality=RendererQuality::Default;
    setRendererQuality(rendererQuality);

    SmartPointer<Picture> pct=Renderer::render(&world,cam,rendererQuality);
    char buf[BUFSIZ];
    sprintf(buf,"rt_%u.ppm",rseed);
    pct->write(buf);

    setAlarm(0);

    exit(0);
}
