#pragma once

#ifdef OpenCL
#include "OpenCLContext.h"

#define CL_TARGET_OPENCL_VERSION 120
#include <CL/cl.h>

class OpenCLKernel
{
public:
    OpenCLKernel(OpenCLContext *ctx,const char* name, const char* source);
    virtual ~OpenCLKernel();

public:
    bool setArg(int index, void *buffer)const;
    bool runKernel(size_t nb)const;

protected:
    size_t getWorkSize(size_t size)const
    {
        return (size_t)((int)size)+(((int)this->workgroupSizeMultiple)-((int)size)%((int)this->workgroupSizeMultiple));
    }

private:
    OpenCLContext *context;
    cl_kernel kernel;
    size_t workgroupSizeMultiple;
};
#endif
