#pragma once

#ifdef OpenCL
#include "Collection.h"

#define CL_TARGET_OPENCL_VERSION 120
#include <CL/cl.h>

class OpenCLKernel:public Lockable
{
public:
    OpenCLKernel(const char* name, const char* source);
    virtual ~OpenCLKernel();

public:
    int createBuffer(size_t nb,size_t size, cl_mem_flags flags);
    bool writeBuffer(int bufferId, size_t nb,size_t size, const void* ptr);
    bool runKernel(size_t nb);
    bool readBuffer(int bufferId, size_t nb,size_t size, void* ptr);

protected:
    size_t getWorkSize(size_t size) const
    {
        return (size_t)((int)size)+(((int)this->workgroupSizeMultiple)-((int)size)%((int)this->workgroupSizeMultiple));
    }

private:
    cl_context context;
    cl_command_queue command_queue;
    cl_kernel kernel;
    size_t computeUnits;
    size_t workgroupSizeMultiple;

    Collection<cl_mem> buffers;
};
#endif
