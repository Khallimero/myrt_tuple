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

void OpenCLKernel::printError(const char* fct, cl_int ret)
{
    switch(ret)
    {
    case CL_SUCCESS:
        break;
    case CL_INVALID_CONTEXT:
        fprintf(stderr,"%s : Invalid context\n",fct);
        break;
    case CL_INVALID_VALUE:
        fprintf(stderr,"%s : Invalid value\n",fct);
        break;
    case CL_INVALID_BUFFER_SIZE:
        fprintf(stderr,"%s : Invalid buffer size\n",fct);
        break;
    case CL_INVALID_HOST_PTR:
        fprintf(stderr,"%s : Invalid host pointer\n",fct);
        break;
    case CL_MEM_OBJECT_ALLOCATION_FAILURE:
        fprintf(stderr,"%s : Mem object allocation failure\n",fct);
        break;
    case CL_OUT_OF_HOST_MEMORY:
        fprintf(stderr,"%s : Out of host memory\n",fct);
        break;
    case CL_INVALID_COMMAND_QUEUE:
        fprintf(stderr,"%s : Invalid command queue\n",fct);
        break;
    case CL_INVALID_MEM_OBJECT:
        fprintf(stderr,"%s : Invalid mem object\n",fct);
        break;
    case CL_INVALID_EVENT_WAIT_LIST:
        fprintf(stderr,"%s : Invalid event wait list\n",fct);
        break;
    case CL_INVALID_PROGRAM_EXECUTABLE:
        fprintf(stderr,"%s : Invalid program executable\n",fct);
        break;
    case CL_INVALID_KERNEL:
        fprintf(stderr,"%s : Invalid kernel\n",fct);
        break;
    case CL_INVALID_KERNEL_ARGS:
        fprintf(stderr,"%s : Invalid kernel args\n",fct);
        break;
    case CL_INVALID_WORK_DIMENSION:
        fprintf(stderr,"%s : Invalid work dimension\n",fct);
        break;
    case CL_INVALID_WORK_GROUP_SIZE:
        fprintf(stderr,"%s : Invalid work group size\n",fct);
        break;
    case CL_INVALID_WORK_ITEM_SIZE:
        fprintf(stderr,"%s : Invalid work item size\n",fct);
        break;
    case CL_INVALID_GLOBAL_OFFSET:
        fprintf(stderr,"%s : Invalid global offset\n",fct);
        break;
    case CL_OUT_OF_RESOURCES:
        fprintf(stderr,"%s : Out of resources\n",fct);
        break;
    case CL_INVALID_ARG_INDEX:
        fprintf(stderr,"%s : Invalid arg index\n",fct);
        break;
    case CL_INVALID_ARG_VALUE:
        fprintf(stderr,"%s : Invalid arg value\n",fct);
        break;
    case CL_INVALID_SAMPLER:
        fprintf(stderr,"%s : Invalid sampler\n",fct);
        break;
    case CL_INVALID_ARG_SIZE:
        fprintf(stderr,"%s : Invalid arg size\n",fct);
        break;
    default:
        fprintf(stderr,"%s : Unknown error\n",fct);
    }
}
#endif
