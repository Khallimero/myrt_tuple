#pragma once

#include "RendererQuality.h"
#include "Scene.h"
#include "Camera.h"
#include "NestedIterator.h"
#include "RoundedIterator.h"
#include "IteratorElement.h"
#include "PhotonMap.h"
#include "PhotonBox.h"
#include "Picture.h"
#include "Hit.h"
#include "Ray.h"
#include "Color.h"
#include "Lockable.h"
#include "Thread.h"
#include "SmartPointer.h"
#include "Collection.h"

#include <time.h>

enum RenderingProgress
{
    Init=0,
    Photon,
    Rendering,
    Complete
};

void* photonThread(void*);
void* renderThread(void*);
void renderThreadCleanup(void*);

class Renderer:public Lockable
{
    friend void* photonThread(void*);
    friend void* renderThread(void*);
    friend void renderThreadCleanup(void*);

protected:
    Renderer(const Scene* sc,const Camera& cam,const RendererQuality& quality);
    virtual ~Renderer();

public:
    static SmartPointer<Picture> render(const Scene* sc,const Camera& cam,
                                        const RendererQuality& quality=RendererQuality::Default);

protected:
    bool getNextPoint(IteratorElement<double>* a,IteratorElement<double>* b);
    Color renderPoint(const Vector& vct)const;
    Color computeRay(const Ray& r,int nbRef=0)const;
    ObjCollection<Color> computeRays(const ObjCollection<Ray>& r,int nbRef=0)const;
    ObjCollection<Color> computeColors(const ObjCollection<Hit>& hc,int nbRef)const;
    Color computeBeerColor(const Color& c,const Hit& h)const;

public:
    const PhotonMap* getPhotonMap()const
    {
        return &photonMap;
    }
    const Picture* getPicture()const
    {
        return pct;
    }

public:
    int getPhotonIteratorSteps()const
    {
        return pIt==NULL?0:pIt->getMaxSteps();
    }

protected:
    bool getNextPhotonPoint(IteratorElement<double>* a,IteratorElement<double>* b);
    void computePhoton(const Hit& h,const Color& col,int nbRef=0);
    void computePhotons(const ObjCollection<Ray>& r,const Color& col,int nbRef=0);

protected:
    void setLightIterator(int n);

public:
    void setRenderingProgress(RenderingProgress rp);
    RenderingProgress getRenderingProgress()const
    {
        return rndProgress;
    }
    int getProgressCount()const
    {
        return progressCount;
    }
    int getRndProgressCount()const
    {
        return rndProgressCount;
    }
    time_t getStartTime()const
    {
        return startTime;
    }

    static Collection<const Renderer*> Monitored;

protected:
    void incProgressCount();
    void setProgressCount(int p);
    void incRndProgressCount();

protected:
    Picture* pct;
    const Scene* sc;
    Camera cam;
    RendererQuality quality;
    mutable PhotonMap photonMap;

    NestedIterator<double,2> nIt;
    RoundedIterator *pIt;

    volatile RenderingProgress rndProgress;
    volatile int progressCount,rndProgressCount;
    time_t startTime;

protected:
    static __thread NestedIterator<double,2>** nestedItTab;
};
