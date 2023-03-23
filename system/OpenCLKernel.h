#pragma once

#ifdef OpenCL

#define CL_TARGET_OPENCL_VERSION 120
#include <CL/cl.h>

class OpenCLKernel
{
public:
    OpenCLKernel(const char* name, const char* source, const char* options=NULL);
    virtual ~OpenCLKernel();

public:
    bool setArg(int index, void *buffer)const;
    bool runKernel(size_t nb,cl_command_queue queue=NULL)const;

protected:
    size_t getWorkSize(size_t size)const
    {
        return (size_t)((int)size)+(((int)this->workgroupSizeMultiple)-((int)size)%((int)this->workgroupSizeMultiple));
    }

private:
    cl_kernel kernel;
    size_t workgroupSizeMultiple;
};
#endif
