#pragma once

#ifdef OpenCL
#include "SmartPointer.h"
#include "OpenCLKernel.h"

class PLYShapeBoxKernel
{
public:
    PLYShapeBoxKernel(int ht, int cnt);
    virtual ~PLYShapeBoxKernel();

public:
    int* getBuffId()
    {
        return buffId;
    }

    void runKernel(int nb);

protected:
    SmartPointer<OpenCLKernel> kernel;
    int buffId[6];
};
#endif
