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
    if(clGetDeviceInfo(device_id, CL_DEVICE_NAME, BUFSIZ,buf, NULL)!=CL_SUCCESS) exit(1);
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
    clGetKernelWorkGroupInfo(kernel, device_id, CL_KERNEL_PREFERRED_WORK_GROUP_SIZE_MULTIPLE, sizeof(size_t),&workgroupSizeMultiple,NULL);
    clReleaseProgram(program);
}

int OpenCLKernel::createBuffer(size_t nb,size_t size, cl_mem_flags flags)
{
    buffers.add(clCreateBuffer(context, flags, getWorkSize(nb)*size, NULL, NULL));
    int bufferId = buffers.count()-1;
    clSetKernelArg(kernel, bufferId, sizeof(cl_mem), (void *)&buffers[bufferId]);
    return bufferId;
}

bool OpenCLKernel::writeBuffer(int bufferId, size_t nb, size_t size, const void* ptr)
{
    return clEnqueueWriteBuffer(command_queue, buffers[bufferId], CL_TRUE, 0, nb*size, ptr, 0, NULL, NULL)==CL_SUCCESS;
}

bool OpenCLKernel::runKernel(size_t nb, cl_int dim)
{
    size_t size=getWorkSize(nb);
    return clEnqueueNDRangeKernel(command_queue, kernel, dim, NULL, &size, &workgroupSizeMultiple, 0, NULL, NULL)==CL_SUCCESS;
}

bool OpenCLKernel::readBuffer(int bufferId, size_t nb, size_t size, void* ptr)
{
    return clEnqueueReadBuffer(command_queue, buffers[bufferId], CL_TRUE, 0, nb*size, ptr, 0, NULL, NULL)==CL_SUCCESS;
}

OpenCLKernel::~OpenCLKernel()
{
    clFlush(command_queue);
    clFinish(command_queue);
    clReleaseKernel(kernel);
    for(int i=0; i<buffers._count(); i++)
        clReleaseMemObject(buffers[i]);
    clReleaseCommandQueue(command_queue);
    clReleaseContext(context);
}
#endif
