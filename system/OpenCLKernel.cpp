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
    fprintf(stdout,"OpenCL device : %s\n",buf);

    cl_int ret;
    context = clCreateContext(NULL, 1, &device_id, NULL, NULL, &ret);
    command_queue = clCreateCommandQueue(context, device_id, 0, &ret);
    size_t source_size = strlen(source);
    cl_program program = clCreateProgramWithSource(context, 1,(const char **)&source, (const size_t *)&source_size, &ret);
    if(clBuildProgram(program, 1, &device_id, NULL, NULL, NULL)!=CL_SUCCESS)
    {
        clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG,BUFSIZ, buf, NULL);
        fprintf(stderr, "%s\n", buf);
        exit(1);
    }
    kernel = clCreateKernel(program, name, &ret);
    clReleaseProgram(program);

    clGetDeviceInfo(device_id, CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(size_t), &computeUnits, NULL);
    clGetKernelWorkGroupInfo(kernel, device_id, CL_KERNEL_PREFERRED_WORK_GROUP_SIZE_MULTIPLE, sizeof(size_t), &workgroupSizeMultiple, NULL);
}

int OpenCLKernel::createBuffer(size_t nb,size_t size, cl_mem_flags flags,bool adjustSize)
{
    buffers.add(clCreateBuffer(context, flags, (adjustSize?getWorkSize(nb):nb)*size, NULL, NULL));
    int bufferId = buffers.count()-1;
    clSetKernelArg(kernel, bufferId, sizeof(cl_mem), (void *)&buffers[bufferId]);
    return bufferId;
}

bool OpenCLKernel::writeBuffer(int bufferId, size_t nb, size_t size, const void* ptr)
{
    return clEnqueueWriteBuffer(command_queue, buffers[bufferId], CL_FALSE, 0, nb*size, ptr, 0, NULL, NULL)==CL_SUCCESS;
}

bool OpenCLKernel::runKernel(size_t nb)
{
    size_t size=(int)getWorkSize(nb);
    size_t workSize=(int)this->workgroupSizeMultiple;
    cl_int ret=clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL, &size, &workSize, 0, NULL, NULL);
    switch(ret)
    {
    case CL_INVALID_PROGRAM_EXECUTABLE:
        fprintf(stderr,"Invalid program executable\n");
        break;
    case CL_INVALID_COMMAND_QUEUE:
        fprintf(stderr,"Invalid command queue\n");
        break;
    case CL_INVALID_KERNEL:
        fprintf(stderr,"Invalid kernel\n");
        break;
    case CL_INVALID_CONTEXT:
        fprintf(stderr,"Invalid context\n");
        break;
    case CL_INVALID_KERNEL_ARGS:
        fprintf(stderr,"Invalid kernel args\n");
        break;
    case CL_INVALID_WORK_DIMENSION:
        fprintf(stderr,"Invalid work dimension\n");
        break;
    case CL_INVALID_WORK_GROUP_SIZE:
        fprintf(stderr,"Invalid work group size\n");
        break;
    case CL_INVALID_WORK_ITEM_SIZE:
        fprintf(stderr,"Invalid work item size\n");
        break;
    case CL_INVALID_GLOBAL_OFFSET:
        fprintf(stderr,"Invalid global offset\n");
        break;
    case CL_OUT_OF_RESOURCES:
        fprintf(stderr,"Out of resources\n");
        break;
    case CL_MEM_OBJECT_ALLOCATION_FAILURE:
        fprintf(stderr,"Mem object allocation failure\n");
        break;
    case CL_INVALID_EVENT_WAIT_LIST:
        fprintf(stderr,"Invalid event wait list\n");
        break;
    case CL_OUT_OF_HOST_MEMORY:
        fprintf(stderr,"Out of host memory\n");
        break;
    }
    return ret==CL_SUCCESS;
}

bool OpenCLKernel::readBuffer(int bufferId, size_t nb, size_t size, void* ptr)
{
    return clEnqueueReadBuffer(command_queue, buffers[bufferId], CL_TRUE, 0, nb*size, ptr, 0, NULL, NULL)==CL_SUCCESS;
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
#endif
