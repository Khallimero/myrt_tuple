#pragma once

#ifdef OpenCL
#include "ConcurrentOpenCLKernel.h"

class PLYShapeBoxKernel : public ConcurrentOpenCLKernel
{
public:
    PLYShapeBoxKernel(int* buffer, int ht, int cnt);
    virtual ~PLYShapeBoxKernel();

public:
    int* getBuffId()
    {
        return buffId;
    }

protected:
    int buffId[4];
};
#endif
