#include "OpenCLKernel.h"

#ifdef OpenCL
#include <string.h>
#include <stdio.h>

OpenCLKernel::OpenCLKernel(OpenCLContext *ctx,const char* name, const char* source, const char* options)
    :context(ctx)
{
    char buf[BUFSIZ];
    cl_int ret;
    size_t source_size = strlen(source);
    cl_program program = clCreateProgramWithSource(context->getContext(), 1,(const char **)&source, (const size_t *)&source_size, &ret);
    OpenCLContext::printError("clCreateProgramWithSource",ret);
    if((ret=clBuildProgram(program, 1, &context->getDeviceId(), options, NULL, NULL))!=CL_SUCCESS)
    {
        OpenCLContext::printError("clBuildProgram",ret);
        ret = clGetProgramBuildInfo(program, context->getDeviceId(), CL_PROGRAM_BUILD_LOG, BUFSIZ, buf, NULL);
        if(ret==CL_SUCCESS)fprintf(stderr, "%s\n", buf);
        exit(1);
    }
    kernel = clCreateKernel(program, name, &ret);
    OpenCLContext::printError("clCreateKernel",ret);
    clReleaseProgram(program);

    clGetKernelWorkGroupInfo(kernel, context->getDeviceId(), CL_KERNEL_PREFERRED_WORK_GROUP_SIZE_MULTIPLE, sizeof(size_t), &workgroupSizeMultiple, NULL);
}

bool OpenCLKernel::setArg(int index, void *buffer)const
{
    cl_int ret=clSetKernelArg(kernel, index, sizeof(cl_mem), buffer);
    OpenCLContext::printError("clSetKernelArg",ret);
    return ret==CL_SUCCESS;
}

bool OpenCLKernel::runKernel(size_t nb)const
{
    size_t size=(int)getWorkSize(nb);
    size_t workSize=(int)this->workgroupSizeMultiple;
    cl_int ret=clEnqueueNDRangeKernel(context->getCommandQueue(), kernel, 1, NULL, &size, &workSize, 0, NULL, NULL);
    OpenCLContext::printError("clEnqueueNDRangeKernel",ret);
    return ret==CL_SUCCESS;
}

OpenCLKernel::~OpenCLKernel()
{
    context->flush();
    clFinish(context->getCommandQueue());
    clReleaseKernel(kernel);
}
#endif
