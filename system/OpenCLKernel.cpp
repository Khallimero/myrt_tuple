#include "OpenCLKernel.h"

#ifdef OpenCL
#include <string.h>
#include <stdio.h>

OpenCLKernel::OpenCLKernel(const char* name, const char* source)
{
    char buf[BUFSIZ];

    cl_platform_id platform_id;
    cl_uint ret_num_platforms;
    if(clGetPlatformIDs(1, &platform_id, &ret_num_platforms)!=CL_SUCCESS)exit(1);
    if(clGetPlatformInfo(platform_id, CL_PLATFORM_NAME, BUFSIZ, buf, NULL)!=CL_SUCCESS)exit(1);
    fprintf(stdout,"OpenCL platform : %s\n",buf);

    cl_device_id device_id;
    cl_uint ret_num_devices;
    if(clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_DEFAULT, 1,&device_id, &ret_num_devices)!=CL_SUCCESS) exit(1);
    if(clGetDeviceInfo(device_id, CL_DEVICE_NAME, BUFSIZ, buf, NULL)!=CL_SUCCESS) exit(1);
    clGetDeviceInfo(device_id, CL_DEVICE_NAME, sizeof(cl_device_type), &device_type, NULL);
    fprintf(stdout,"OpenCL device : %s\n",buf);

    cl_int ret;
    context = clCreateContext(NULL, 1, &device_id, NULL, NULL, &ret);
    printError("clCreateContext",ret);
    command_queue = clCreateCommandQueue(context, device_id, 0, &ret);
    printError("clCreateCommandQueue",ret);
    size_t source_size = strlen(source);
    cl_program program = clCreateProgramWithSource(context, 1,(const char **)&source, (const size_t *)&source_size, &ret);
    printError("clCreateProgramWithSource",ret);
    if(clBuildProgram(program, 1, &device_id, NULL, NULL, NULL)!=CL_SUCCESS)
    {
        clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG,BUFSIZ, buf, NULL);
        fprintf(stderr, "%s\n", buf);
        exit(1);
    }
    kernel = clCreateKernel(program, name, &ret);
    printError("clCreateKernel",ret);
    clReleaseProgram(program);

    clGetDeviceInfo(device_id, CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(size_t), &computeUnits, NULL);
    clGetKernelWorkGroupInfo(kernel, device_id, CL_KERNEL_PREFERRED_WORK_GROUP_SIZE_MULTIPLE, sizeof(size_t), &workgroupSizeMultiple, NULL);
    fprintf(stdout,"Workgroup size : %d\n",(int)workgroupSizeMultiple);
}

int OpenCLKernel::createBuffer(size_t nb,size_t size, cl_mem_flags flags,bool adjustSize)
{
    cl_int ret;
    buffers.add(clCreateBuffer(context, flags, (adjustSize?getWorkSize(nb):nb)*size, NULL, &ret));
    printError("clCreateBuffer",ret);
    int bufferId = buffers.count()-1;
    ret=clSetKernelArg(kernel, bufferId, sizeof(cl_mem), (void *)&buffers[bufferId]);
    printError("clSetKernelArg",ret);
    return bufferId;
}

bool OpenCLKernel::writeBuffer(int bufferId, size_t nb, size_t size, const void* ptr)
{
    cl_int ret=clEnqueueWriteBuffer(command_queue, buffers[bufferId], CL_TRUE, 0, nb*size, ptr, 0, NULL, NULL);
    printError("clEnqueueWriteBuffer",ret);
    return ret==CL_SUCCESS;
}

bool OpenCLKernel::runKernel(size_t nb)
{
    size_t size=(int)getWorkSize(nb);
    size_t workSize=(int)this->workgroupSizeMultiple;
    cl_int ret=clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL, &size, &workSize, 0, NULL, NULL);
    printError("clEnqueueNDRangeKernel",ret);
    return ret==CL_SUCCESS;
}

bool OpenCLKernel::readBuffer(int bufferId, size_t nb, size_t size, void* ptr)
{
    cl_int ret=clEnqueueReadBuffer(command_queue, buffers[bufferId], CL_TRUE, 0, nb*size, ptr, 0, NULL, NULL);
    printError("clEnqueueReadBuffer",ret);
    return ret==CL_SUCCESS;
}

void OpenCLKernel::flush()
{
    clFlush(command_queue);
}

OpenCLKernel::~OpenCLKernel()
{
    flush();
    clFinish(command_queue);
    clReleaseKernel(kernel);
    for(int i=0; i<buffers._count(); i++)
        clReleaseMemObject(buffers[i]);
    clReleaseCommandQueue(command_queue);
    clReleaseContext(context);
}

#define CaseErrorCode(x) case x: fprintf(stderr,"%s : %s\n",fct,#x);break
void OpenCLKernel::printError(const char* fct, cl_int ret)
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
