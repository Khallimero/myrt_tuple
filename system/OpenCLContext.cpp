#include "OpenCLContext.h"

#ifdef OpenCL
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
    cl_int ret;
    buffers.add(clCreateBuffer(context, flags, nb*size, NULL, &ret));
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

#define CaseErrorCode(x) case x: fprintf(stderr,"%s : %s\n",fct,#x);break
void OpenCLContext::printError(const char* fct, cl_int ret)
{
    switch(ret)
    {
    case CL_SUCCESS:
        break;
        CaseErrorCode(CL_DEVICE_NOT_FOUND);
        CaseErrorCode(CL_DEVICE_NOT_AVAILABLE);
        CaseErrorCode(CL_COMPILER_NOT_AVAILABLE);
        CaseErrorCode(CL_MEM_OBJECT_ALLOCATION_FAILURE);
        CaseErrorCode(CL_OUT_OF_RESOURCES);
        CaseErrorCode(CL_OUT_OF_HOST_MEMORY);
        CaseErrorCode(CL_PROFILING_INFO_NOT_AVAILABLE);
        CaseErrorCode(CL_MEM_COPY_OVERLAP);
        CaseErrorCode(CL_IMAGE_FORMAT_MISMATCH);
        CaseErrorCode(CL_IMAGE_FORMAT_NOT_SUPPORTED);
        CaseErrorCode(CL_BUILD_PROGRAM_FAILURE);
        CaseErrorCode(CL_MAP_FAILURE);
        CaseErrorCode(CL_MISALIGNED_SUB_BUFFER_OFFSET);
        CaseErrorCode(CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST);
        CaseErrorCode(CL_COMPILE_PROGRAM_FAILURE);
        CaseErrorCode(CL_LINKER_NOT_AVAILABLE);
        CaseErrorCode(CL_LINK_PROGRAM_FAILURE);
        CaseErrorCode(CL_DEVICE_PARTITION_FAILED);
        CaseErrorCode(CL_KERNEL_ARG_INFO_NOT_AVAILABLE);
        CaseErrorCode(CL_INVALID_VALUE);
        CaseErrorCode(CL_INVALID_DEVICE_TYPE);
        CaseErrorCode(CL_INVALID_PLATFORM);
        CaseErrorCode(CL_INVALID_DEVICE);
        CaseErrorCode(CL_INVALID_CONTEXT);
        CaseErrorCode(CL_INVALID_QUEUE_PROPERTIES);
        CaseErrorCode(CL_INVALID_COMMAND_QUEUE);
        CaseErrorCode(CL_INVALID_HOST_PTR);
        CaseErrorCode(CL_INVALID_MEM_OBJECT);
        CaseErrorCode(CL_INVALID_IMAGE_FORMAT_DESCRIPTOR);
        CaseErrorCode(CL_INVALID_IMAGE_SIZE);
        CaseErrorCode(CL_INVALID_SAMPLER);
        CaseErrorCode(CL_INVALID_BINARY);
        CaseErrorCode(CL_INVALID_BUILD_OPTIONS);
        CaseErrorCode(CL_INVALID_PROGRAM);
        CaseErrorCode(CL_INVALID_PROGRAM_EXECUTABLE);
        CaseErrorCode(CL_INVALID_KERNEL_NAME);
        CaseErrorCode(CL_INVALID_KERNEL_DEFINITION);
        CaseErrorCode(CL_INVALID_KERNEL);
        CaseErrorCode(CL_INVALID_ARG_INDEX);
        CaseErrorCode(CL_INVALID_ARG_VALUE);
        CaseErrorCode(CL_INVALID_ARG_SIZE);
        CaseErrorCode(CL_INVALID_KERNEL_ARGS);
        CaseErrorCode(CL_INVALID_WORK_DIMENSION);
        CaseErrorCode(CL_INVALID_WORK_GROUP_SIZE);
        CaseErrorCode(CL_INVALID_WORK_ITEM_SIZE);
        CaseErrorCode(CL_INVALID_GLOBAL_OFFSET);
        CaseErrorCode(CL_INVALID_EVENT_WAIT_LIST);
        CaseErrorCode(CL_INVALID_EVENT);
        CaseErrorCode(CL_INVALID_OPERATION);
        CaseErrorCode(CL_INVALID_GL_OBJECT);
        CaseErrorCode(CL_INVALID_BUFFER_SIZE);
        CaseErrorCode(CL_INVALID_MIP_LEVEL);
        CaseErrorCode(CL_INVALID_GLOBAL_WORK_SIZE);
        CaseErrorCode(CL_INVALID_PROPERTY);
        CaseErrorCode(CL_INVALID_IMAGE_DESCRIPTOR);
        CaseErrorCode(CL_INVALID_COMPILER_OPTIONS);
        CaseErrorCode(CL_INVALID_LINKER_OPTIONS);
        CaseErrorCode(CL_INVALID_DEVICE_PARTITION_COUNT);
    default:
        fprintf(stderr,"%s : Unknown error\n",fct);
    }
}
#endif
