#pragma once

#ifdef OpenCL
#include "Lockable.h"
#include "LockQueue.h"
#include "Collection.h"
#include "SmartPointer.h"

#define CL_TARGET_OPENCL_VERSION 120
#include <CL/cl.h>

class OpenCLContext:public Lockable
{
protected:
    OpenCLContext();
public:
    virtual ~OpenCLContext();

public:
    int createBuffer(size_t nb,size_t size, cl_mem_flags flags);
    bool writeBuffer(int bufferId, size_t nb,size_t size, const void* ptr)const;
    bool readBuffer(int bufferId, size_t nb,size_t size, void* ptr)const;
    void* getBuffer(int bufferId)
    {
        return (void*)&buffers[bufferId];
    }
    void releaseBuffer(int bufferId);

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
    bool finish()const;

public:
    static SmartPointer<OpenCLContext> openCLcontext;
    static LockQueue<OPENCL_QUEUE> openCLQueue;

protected:
    cl_device_id device_id;
    cl_context context;
    cl_command_queue command_queue;

    Collection<cl_mem> buffers;
};
#endif
