#include "PLYShapeBoxKernel.h"

#ifdef OpenCL
#include "core.h"
#include "OpenCLContext.h"

PLYShapeBoxKernel::PLYShapeBoxKernel(const int* buffer, int ht, int cnt)
    :ConcurrentOpenCLKernel(true)
{
    this->kernel=new OpenCLKernel("primitive_box", "#pragma OPENCL EXTENSION cl_khr_fp64 : enable\n\
__kernel void primitive_box(\
    __global const double *prm,\
    __global const int *cnt_prm,\
    __global const double *ht,\
    __global const int *cnt_ht,\
    __global const double *box,\
    __global int *h)\
{\
    int id=get_global_id(0);\
    if(id>=(*cnt_prm))return;\
    for(int i=0;i<3;i++)\
        if(length(vload3(0,box)-vload3((id*3)+i,prm))<=box[3])\
        {\
            int nb=0;\
            for(int j=0;nb<3&&j<*cnt_ht;j++)\
            {\
                int n=0;\
                for(int k=0;k<3;k++)\
                    for(int l=k;l<3;l++)\
                        if(length(vload3((id*3)+k,prm)-vload3((j*3)+l,ht))==0)n++;\
                nb=max(nb,n);\
            }\
            if(nb>0&&nb<3)h[1+atomic_inc(h)]=id;\
            return;\
        }\
}");

    for(int i=0; i<2; i++)
        this->kernel->setArg(i, OpenCLContext::openCLcontext->getBuffer(buffer[i]));

    buffId[0]=OpenCLContext::openCLcontext->createBuffer(ht*3*TREBLE_SIZE,sizeof(double),CL_MEM_READ_ONLY);
    buffId[1]=OpenCLContext::openCLcontext->createBuffer(1,sizeof(int),CL_MEM_READ_ONLY);
    buffId[2]=OpenCLContext::openCLcontext->createBuffer(TREBLE_SIZE+1,sizeof(double),CL_MEM_READ_ONLY);
    buffId[3]=OpenCLContext::openCLcontext->createBuffer(cnt,sizeof(int),CL_MEM_READ_WRITE);

    for(int i=0; i<4; i++)
        this->kernel->setArg(i+2, OpenCLContext::openCLcontext->getBuffer(buffId[i]));
}

PLYShapeBoxKernel::~PLYShapeBoxKernel()
{
    for(int i=0; i<4; i++)
        OpenCLContext::openCLcontext->releaseBuffer(buffId[i]);
}
#endif
