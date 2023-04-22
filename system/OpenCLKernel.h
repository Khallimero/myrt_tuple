#pragma once

#ifdef OpenCL

#define CL_TARGET_OPENCL_VERSION 120
#include <CL/cl.h>

#include "OpenCLContext.h"

class OpenCLKernel
{
public:
    OpenCLKernel(const char* name, const char* source, const char* options=NULL);
    virtual ~OpenCLKernel();

public:
    bool setArg(int index, void *buffer)const;

    template <int N> bool runKernel(const int* nb,cl_command_queue queue=NULL)const
    {
        size_t size[N],workSize[N];
        for(int n=0; n<N; n++)
        {
            size[n]=(n==0?getWorkSize(nb[n]):nb[n]);
            workSize[n]=(n==0?this->workgroupSizeMultiple:1);
        }
        if(queue==NULL)queue=OpenCLContext::openCLcontext->getCommandQueue();
        cl_int ret=clEnqueueNDRangeKernel(queue, kernel, N, NULL, size, workSize, 0, NULL, NULL);
        OpenCLContext::printError("clEnqueueNDRangeKernel",ret);
        return ret==CL_SUCCESS;
    }

protected:
    size_t getWorkSize(size_t size)const
    {
        return (size_t)((int)size-1)+(((int)this->workgroupSizeMultiple)-((int)size-1)%((int)this->workgroupSizeMultiple));
    }

private:
    cl_kernel kernel;
    size_t workgroupSizeMultiple;
};
#endif
