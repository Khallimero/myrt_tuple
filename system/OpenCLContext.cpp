#include "OpenCLContext.h"

#ifdef OpenCL
#include "AutoLock.h"

#include <stdio.h>

OpenCLContext::OpenCLContext()
    :Lockable()
{
    char buf[BUFSIZ];

    cl_platform_id platform_id;
    cl_uint ret_num_platforms;
    if(clGetPlatformIDs(1, &platform_id, &ret_num_platforms)!=CL_SUCCESS)exit(1);
    if(clGetPlatformInfo(platform_id, CL_PLATFORM_NAME, BUFSIZ, buf, NULL)!=CL_SUCCESS)exit(1);
    fprintf(stdout,"OpenCL platform : %s\n",buf);

    cl_uint ret_num_devices;
    if(clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_DEFAULT, 1,&device_id, &ret_num_devices)!=CL_SUCCESS) exit(1);
    if(clGetDeviceInfo(device_id, CL_DEVICE_NAME, BUFSIZ, buf, NULL)!=CL_SUCCESS) exit(1);
    clGetDeviceInfo(device_id, CL_DEVICE_TYPE, sizeof(cl_device_type), &device_type, NULL);
    fprintf(stdout,"OpenCL device : %s\n",buf);

    cl_int ret;
    context = clCreateContext(NULL, 1, &device_id, NULL, NULL, &ret);
    printError("clCreateContext",ret);
    command_queue = clCreateCommandQueue(context, device_id, 0, &ret);
    printError("clCreateCommandQueue",ret);
}

OpenCLContext::~OpenCLContext()
{
    for(int i=0; i<buffers._count(); i++)
        clReleaseMemObject(buffers[i]);
    clReleaseCommandQueue(command_queue);
    clReleaseContext(context);
}

int OpenCLContext::createBuffer(size_t nb,size_t size, cl_mem_flags flags)
{
    AutoLock lock(&buffers);
    cl_int ret;
    buffers._add(clCreateBuffer(context, flags, nb*size, NULL, &ret));
    printError("clCreateBuffer",ret);
    return buffers._count()-1;
}

bool OpenCLContext::writeBuffer(int bufferId, size_t nb, size_t size, const void* ptr)const
{
    cl_int ret=clEnqueueWriteBuffer(command_queue, buffers[bufferId], CL_TRUE, 0, nb*size, ptr, 0, NULL, NULL);
    printError("clEnqueueWriteBuffer",ret);
    return ret==CL_SUCCESS;
}

bool OpenCLContext::readBuffer(int bufferId, size_t nb, size_t size, void* ptr)const
{
    cl_int ret=clEnqueueReadBuffer(command_queue, buffers[bufferId], CL_TRUE, 0, nb*size, ptr, 0, NULL, NULL);
    printError("clEnqueueReadBuffer",ret);
    return ret==CL_SUCCESS;
}

bool OpenCLContext::flush()const
{
    return clFlush(command_queue)==CL_SUCCESS;
}

#define ErrorCode(x) x: fprintf(stderr,"%s : %s\n",fct,#x);break
void OpenCLContext::printError(const char* fct, cl_int ret)
{
    switch(ret)
    {
    case CL_SUCCESS:
        break;
    case ErrorCode(CL_DEVICE_NOT_FOUND);
    case ErrorCode(CL_DEVICE_NOT_AVAILABLE);
    case ErrorCode(CL_COMPILER_NOT_AVAILABLE);
    case ErrorCode(CL_MEM_OBJECT_ALLOCATION_FAILURE);
    case ErrorCode(CL_OUT_OF_RESOURCES);
    case ErrorCode(CL_OUT_OF_HOST_MEMORY);
    case ErrorCode(CL_PROFILING_INFO_NOT_AVAILABLE);
    case ErrorCode(CL_MEM_COPY_OVERLAP);
    case ErrorCode(CL_IMAGE_FORMAT_MISMATCH);
    case ErrorCode(CL_IMAGE_FORMAT_NOT_SUPPORTED);
    case ErrorCode(CL_BUILD_PROGRAM_FAILURE);
    case ErrorCode(CL_MAP_FAILURE);
    case ErrorCode(CL_MISALIGNED_SUB_BUFFER_OFFSET);
    case ErrorCode(CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST);
    case ErrorCode(CL_COMPILE_PROGRAM_FAILURE);
    case ErrorCode(CL_LINKER_NOT_AVAILABLE);
    case ErrorCode(CL_LINK_PROGRAM_FAILURE);
    case ErrorCode(CL_DEVICE_PARTITION_FAILED);
    case ErrorCode(CL_KERNEL_ARG_INFO_NOT_AVAILABLE);
    case ErrorCode(CL_INVALID_VALUE);
    case ErrorCode(CL_INVALID_DEVICE_TYPE);
    case ErrorCode(CL_INVALID_PLATFORM);
    case ErrorCode(CL_INVALID_DEVICE);
    case ErrorCode(CL_INVALID_CONTEXT);
    case ErrorCode(CL_INVALID_QUEUE_PROPERTIES);
    case ErrorCode(CL_INVALID_COMMAND_QUEUE);
    case ErrorCode(CL_INVALID_HOST_PTR);
    case ErrorCode(CL_INVALID_MEM_OBJECT);
    case ErrorCode(CL_INVALID_IMAGE_FORMAT_DESCRIPTOR);
    case ErrorCode(CL_INVALID_IMAGE_SIZE);
    case ErrorCode(CL_INVALID_SAMPLER);
    case ErrorCode(CL_INVALID_BINARY);
    case ErrorCode(CL_INVALID_BUILD_OPTIONS);
    case ErrorCode(CL_INVALID_PROGRAM);
    case ErrorCode(CL_INVALID_PROGRAM_EXECUTABLE);
    case ErrorCode(CL_INVALID_KERNEL_NAME);
    case ErrorCode(CL_INVALID_KERNEL_DEFINITION);
    case ErrorCode(CL_INVALID_KERNEL);
    case ErrorCode(CL_INVALID_ARG_INDEX);
    case ErrorCode(CL_INVALID_ARG_VALUE);
    case ErrorCode(CL_INVALID_ARG_SIZE);
    case ErrorCode(CL_INVALID_KERNEL_ARGS);
    case ErrorCode(CL_INVALID_WORK_DIMENSION);
    case ErrorCode(CL_INVALID_WORK_GROUP_SIZE);
    case ErrorCode(CL_INVALID_WORK_ITEM_SIZE);
    case ErrorCode(CL_INVALID_GLOBAL_OFFSET);
    case ErrorCode(CL_INVALID_EVENT_WAIT_LIST);
    case ErrorCode(CL_INVALID_EVENT);
    case ErrorCode(CL_INVALID_OPERATION);
    case ErrorCode(CL_INVALID_GL_OBJECT);
    case ErrorCode(CL_INVALID_BUFFER_SIZE);
    case ErrorCode(CL_INVALID_MIP_LEVEL);
    case ErrorCode(CL_INVALID_GLOBAL_WORK_SIZE);
    case ErrorCode(CL_INVALID_PROPERTY);
    case ErrorCode(CL_INVALID_IMAGE_DESCRIPTOR);
    case ErrorCode(CL_INVALID_COMPILER_OPTIONS);
    case ErrorCode(CL_INVALID_LINKER_OPTIONS);
    case ErrorCode(CL_INVALID_DEVICE_PARTITION_COUNT);
    default:
        fprintf(stderr,"%s : Unknown error\n",fct);
    }
}

SmartPointer<OpenCLContext> OpenCLContext::openCLcontext=new OpenCLContext();
LockQueue<OpenCL_Queue> OpenCLContext::openCLQueue(OpenCLContext::openCLcontext);
#endif
