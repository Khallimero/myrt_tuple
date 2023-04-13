#pragma once

#ifdef OpenCL
#include "ConcurrentOpenCLKernel.h"

class PLYShapeHitKernel : public ConcurrentOpenCLKernel
{
public:
    PLYShapeHitKernel(int buffer,int cnt);
    virtual ~PLYShapeHitKernel();

public:
    int* getBuffId()
    {
        return buffId;
    }

    int getNbHit()const
    {
        return nb_hit;
    }

    void setNbRay(int nb);
    void setNbHit(int nb);

protected:
    void releaseBuffer(int b);
    void setKernelArg(int b);

protected:
    int buffId[3];
    int nb_ray,nb_hit;
};
#endif
