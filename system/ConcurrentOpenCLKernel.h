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

    void runKernel(int nb);

protected:
    int threadId;
    SmartPointer<OpenCLKernel> kernel;
    cl_command_queue command_queue;
};
#endif
