#include "PLYShapeHitKernel.h"

#ifdef OpenCL
#include "core.h"
#include "OpenCLContext.h"

PLYShapeHitKernel::PLYShapeHitKernel(int buffer,int cnt)
    :ConcurrentOpenCLKernel(true),nb_ray(0),nb_hit(0)
{
    this->kernel=new OpenCLKernel("primitive_hit", "#pragma OPENCL EXTENSION cl_khr_fp64 : enable\n\
__kernel void primitive_hit(\
    __global const double *prm,\
    __global const int *cnt,\
    __global const double *r,\
    __global int *k)\
{\
    int id=get_global_id(0),sid=0;\
    for(int i=1;i<=cnt[0]&&sid<=id;sid+=abs(cnt[i++]))\
        if(cnt[i]<0)id-=cnt[i];\
    if(id>=sid)return;\
    double3 t_o=vload3(id*3,prm);\
    double3 t_v1=vload3((id*3)+1,prm);\
    double3 t_v2=vload3((id*3)+2,prm);\
    for(int ir=0;ir<cnt[cnt[0]+1];ir++)\
    {\
        double3 t_pt=vload3(ir*2,r);\
        double3 t_vct=vload3(ir*2+1,r);\
        double3 t_w=t_pt-t_o;\
        double d=dot(cross(t_v2,t_v1),t_vct);\
        if(dot(cross(t_v1,t_v2),t_w)/d>0.0)\
        {\
            double a=dot(cross(t_v2,t_w),t_vct)/d;\
            double b=dot(cross(t_w,t_v1),t_vct)/d;\
            if(a>=0.0&&b>=0.0&&(a+b)<=1.0)\
            {\
                int ik=atomic_inc(k);\
                if(ik<cnt[cnt[0]+2])\
                    vstore2((int2)(ir,id),ik,k+1);\
            }\
        }\
    }\
}");

    this->kernel->setArg(0, OpenCLContext::openCLcontext->getBuffer(buffer));

    buffId[0]=OpenCLContext::openCLcontext->createBuffer(1+cnt+2,sizeof(int),CL_MEM_READ_ONLY);
    setKernelArg(0);

    setNbRay(1);
}

PLYShapeHitKernel::~PLYShapeHitKernel()
{
    for(int i=0; i<3; i++)
        releaseBuffer(i);
}

void PLYShapeHitKernel::setKernelArg(int b)
{
    this->kernel->setArg(b+1, OpenCLContext::openCLcontext->getBuffer(buffId[b]));
}

void PLYShapeHitKernel::releaseBuffer(int b)
{
    OpenCLContext::openCLcontext->releaseBuffer(buffId[b]);
}

void PLYShapeHitKernel::setNbRay(int nb)
{
    if(nb>nb_ray)
    {
        if(nb_ray>0)releaseBuffer(1);
        nb_ray=nb;
        buffId[1]=OpenCLContext::openCLcontext->createBuffer(nb_ray*2*TREBLE_SIZE,sizeof(double),CL_MEM_READ_ONLY);
        setKernelArg(1);
    }
    setNbHit(2*nb_ray);
}

void PLYShapeHitKernel::setNbHit(int nb)
{
    if(nb>nb_hit)
    {
        if(nb_hit>0)releaseBuffer(2);
        nb_hit=nb;
        buffId[2]=OpenCLContext::openCLcontext->createBuffer(1+(nb_hit*2),sizeof(int),CL_MEM_READ_WRITE);
        setKernelArg(2);
    }
}
#endif
