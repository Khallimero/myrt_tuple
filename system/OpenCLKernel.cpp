#include "OpenCLKernel.h"

#ifdef OpenCL
#include "OpenCLContext.h"

#include <string.h>
#include <stdio.h>

OpenCLKernel::OpenCLKernel(const char* name, const char* source, const char* options)
{
    char buf[BUFSIZ];
    cl_int ret;
    size_t source_size = strlen(source);
    cl_program program = clCreateProgramWithSource(OpenCLContext::openCLcontext->getContext(), 1,(const char **)&source, (const size_t *)&source_size, &ret);
    OpenCLContext::printError("clCreateProgramWithSource",ret);
    if((ret=clBuildProgram(program, 1, &OpenCLContext::openCLcontext->getDeviceId(), options, NULL, NULL))!=CL_SUCCESS)
    {
        if(clGetProgramBuildInfo(program, OpenCLContext::openCLcontext->getDeviceId(), CL_PROGRAM_BUILD_LOG, BUFSIZ, buf, NULL)==CL_SUCCESS)
        {
            fprintf(stderr, "%s\n", buf);
            fflush(stderr);
        }
        OpenCLContext::printError("clBuildProgram",ret);
    }
    kernel = clCreateKernel(program, name, &ret);
    OpenCLContext::printError("clCreateKernel",ret);
    clReleaseProgram(program);

    clGetKernelWorkGroupInfo(kernel, OpenCLContext::openCLcontext->getDeviceId(), CL_KERNEL_PREFERRED_WORK_GROUP_SIZE_MULTIPLE, sizeof(size_t), &workgroupSizeMultiple, NULL);
}

bool OpenCLKernel::setArg(int index, void *buffer)const
{
    cl_int ret=clSetKernelArg(kernel, index, sizeof(cl_mem), buffer);
    OpenCLContext::printError("clSetKernelArg",ret);
    return ret==CL_SUCCESS;
}

bool OpenCLKernel::runKernel(size_t nb,cl_command_queue queue)const
{
    size_t size=(int)getWorkSize(nb);
    size_t workSize=(int)this->workgroupSizeMultiple;
    if(queue==NULL)queue=OpenCLContext::openCLcontext->getCommandQueue();
    cl_int ret=clEnqueueNDRangeKernel(queue, kernel, 1, NULL, &size, &workSize, 0, NULL, NULL);
    OpenCLContext::printError("clEnqueueNDRangeKernel",ret);
    return ret==CL_SUCCESS;
}

OpenCLKernel::~OpenCLKernel()
{
    clReleaseKernel(kernel);
}
#endif
