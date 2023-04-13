#include "ConcurrentOpenCLKernel.h"

#ifdef OpenCL
#include "Thread.h"

ConcurrentOpenCLKernel::ConcurrentOpenCLKernel(bool createCommandQueue)
{
    threadId=Thread::threadId();

    command_queue=createCommandQueue ? OpenCLContext::openCLcontext->createCommandQueue() : NULL;
}

ConcurrentOpenCLKernel::~ConcurrentOpenCLKernel()
{
    if(command_queue!=NULL) clReleaseCommandQueue(command_queue);
}
#endif
