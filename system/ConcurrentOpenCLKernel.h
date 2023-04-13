#pragma once

#ifdef OpenCL
#include "OpenCLContext.h"
#include "OpenCLKernel.h"

class ConcurrentOpenCLKernel
{
public:
    ConcurrentOpenCLKernel(bool createCommandQueue=false);
    virtual ~ConcurrentOpenCLKernel();

public:
    int getThreadId()const
    {
        return threadId;
    }

    cl_command_queue getCommandQueue()
    {
        return command_queue;
    }

    template <int N> void runKernel(const int *nb)
    {
        kernel->runKernel<N>(nb, command_queue);
    }

protected:
    int threadId;
    SmartPointer<OpenCLKernel> kernel;
    cl_command_queue command_queue;
};
#endif
