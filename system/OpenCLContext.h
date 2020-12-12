#pragma once

#ifdef OpenCL
#include "Lockable.h"
#include "Collection.h"

#define CL_TARGET_OPENCL_VERSION 120
#include <CL/cl.h>

class OpenCLContext:public Lockable
{
public:
    OpenCLContext();
    virtual ~OpenCLContext();

public:
    bool isCPU()const
    {
        return device_type==CL_DEVICE_TYPE_CPU;
    }
    bool isGPU()const
    {
        return device_type==CL_DEVICE_TYPE_GPU;
    }

public:
    int createBuffer(size_t nb,size_t size, cl_mem_flags flags);
    bool writeBuffer(int bufferId, size_t nb,size_t size, const void* ptr)const;
    bool readBuffer(int bufferId, size_t nb,size_t size, void* ptr)const;
    void* getBuffer(int bufferId)
    {
        return (void*)&buffers[bufferId];
    }

public:
    static void printError(const char* fct,cl_int ret);

public:
    cl_context& getContext()
    {
        return context;
    }
    cl_device_id& getDeviceId()
    {
        return device_id;
    }
    cl_command_queue& getCommandQueue()
    {
        return command_queue;
    }
    bool flush()const;

protected:
    cl_device_id device_id;
    cl_device_type device_type;
    cl_context context;
    cl_command_queue command_queue;

    Collection<cl_mem> buffers;
};
#endif
