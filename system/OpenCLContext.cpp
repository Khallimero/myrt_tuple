#include "OpenCLContext.h"

#ifdef OpenCL
#include "AutoLock.h"

#include <stdio.h>

OpenCLContext::OpenCLContext()
    :Lockable()
{
    char buf[BUFSIZ];

    cl_int ret;
    cl_platform_id platform_id;
    cl_uint ret_num_platforms;
    ret=clGetPlatformIDs(1, &platform_id, &ret_num_platforms);
    printError("clGetPlatformIDs",ret);
    ret=clGetPlatformInfo(platform_id, CL_PLATFORM_NAME, BUFSIZ, buf, NULL);
    printError("clGetPlatformInfo",ret);
    fprintf(stdout,"OpenCL platform : %s\n",buf);

    cl_uint ret_num_devices;
    ret=clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_DEFAULT, 1, &device_id, &ret_num_devices);
    printError("clGetDeviceIDs",ret);
    ret=clGetDeviceInfo(device_id, CL_DEVICE_NAME, BUFSIZ, buf, NULL);
    printError("clGetDeviceInfo",ret);
    fprintf(stdout,"OpenCL device : %s\n",buf);

    context = clCreateContext(NULL, 1, &device_id, NULL, NULL, &ret);
    printError("clCreateContext",ret);
    command_queue = createCommandQueue();

    fprintf(stdout, "OpenCL queue length : %d\n", OPENCL_QUEUE);
    fprintf(stdout, "OpenCL concurrent kernels : %d\n", OPENCL_CONCURRENTLOCK);
    fflush(stdout);
}

OpenCLContext::~OpenCLContext()
{
    for(int i=0; i<buffers._count(); i++)
        if(buffers[i]!=NULL)
            clReleaseMemObject(buffers[i]);
    clReleaseCommandQueue(command_queue);
    clReleaseContext(context);
}

cl_command_queue OpenCLContext::createCommandQueue(bool ordered)
{
    cl_int ret;
    cl_command_queue queue = clCreateCommandQueue(context, device_id, ordered ? 0 : CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE, &ret);
    printError("clCreateCommandQueue",ret);

    return queue;
}

int OpenCLContext::createBuffer(size_t nb,size_t size, cl_mem_flags flags)
{
    AutoLock lock(&buffers);
    cl_int ret;
    buffers._add(clCreateBuffer(context, flags, nb*size, NULL, &ret));
    printError("clCreateBuffer",ret);
    return buffers._count()-1;
}

bool OpenCLContext::writeBuffer(int bufferId, size_t nb, size_t size, const void* ptr,cl_command_queue queue)const
{
    if(queue==NULL)queue=command_queue;
    cl_int ret=clEnqueueWriteBuffer(queue, buffers[bufferId], CL_TRUE, 0, nb*size, ptr, 0, NULL, NULL);
    printError("clEnqueueWriteBuffer",ret);
    return ret==CL_SUCCESS;
}

bool OpenCLContext::readBuffer(int bufferId, size_t nb, size_t size, void* ptr,cl_command_queue queue)const
{
    if(queue==NULL)queue=command_queue;
    cl_int ret=clEnqueueReadBuffer(queue, buffers[bufferId], CL_TRUE, 0, nb*size, ptr, 0, NULL, NULL);
    printError("clEnqueueReadBuffer",ret);
    return ret==CL_SUCCESS;
}

void OpenCLContext::releaseBuffer(int bufferId)
{
    if(buffers[bufferId]!=NULL)
    {
        clReleaseMemObject(buffers[bufferId]);
        buffers.getTab()[bufferId]=NULL;
    }
}

bool OpenCLContext::flush(cl_command_queue queue)const
{
    if(queue==NULL)queue=command_queue;
    return clFlush(queue)==CL_SUCCESS;
}

bool OpenCLContext::finish(cl_command_queue queue)const
{
    if(queue==NULL)queue=command_queue;
    return flush(queue) && (clFinish(queue)==CL_SUCCESS);
}

#define ErrorCode(x) x: fprintf(stderr,"%s : %s\n",fct,#x);fflush(stderr);exit(1);break
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
        fprintf(stderr,"%s : Unknown error %d\n",fct,ret);
        fflush(stderr);
        exit(1);
    }
}

SmartPointer<OpenCLContext> OpenCLContext::openCLcontext=new OpenCLContext();
LockQueue<OPENCL_QUEUE> OpenCLContext::openCLQueue(OpenCLContext::openCLcontext);
ConcurrentLock<OPENCL_CONCURRENTLOCK> OpenCLContext::concurrentLock;
#endif
