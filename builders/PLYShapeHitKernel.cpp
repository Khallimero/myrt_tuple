#include "PLYShapeHitKernel.h"

#ifdef OpenCL
#include "core.h"
#include "OpenCLContext.h"

PLYShapeHitKernel::PLYShapeHitKernel(int buffer,int cnt)
    :ConcurrentOpenCLKernel(true)
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
        if(cnt[i]<0)id+=abs(cnt[i]);\
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

    nb_ray=1,nb_hit=0;
    buffId[0]=OpenCLContext::openCLcontext->createBuffer(1+cnt+2,sizeof(int),CL_MEM_READ_ONLY);
    buffId[1]=OpenCLContext::openCLcontext->createBuffer(nb_ray*2*TREBLE_SIZE,sizeof(double),CL_MEM_READ_ONLY);
    buffId[2]=OpenCLContext::openCLcontext->createBuffer(1+(nb_hit*2),sizeof(int),CL_MEM_READ_WRITE);

    for(int i=0; i<3; i++)
        this->kernel->setArg(i+1, OpenCLContext::openCLcontext->getBuffer(buffId[i]));
}

PLYShapeHitKernel::~PLYShapeHitKernel()
{
    for(int i=0; i<3; i++)
        OpenCLContext::openCLcontext->releaseBuffer(buffId[i]);
}

void PLYShapeHitKernel::setNbRay(int nb)
{
    nb_ray=nb;
    OpenCLContext::openCLcontext->releaseBuffer(buffId[1]);
    buffId[1]=OpenCLContext::openCLcontext->createBuffer(nb_ray*2*TREBLE_SIZE,sizeof(double),CL_MEM_READ_ONLY);
    this->kernel->setArg(2, OpenCLContext::openCLcontext->getBuffer(buffId[1]));
}

void PLYShapeHitKernel::setNbHit(int nb)
{
    nb_hit=nb;
    OpenCLContext::openCLcontext->releaseBuffer(buffId[2]);
    buffId[2]=OpenCLContext::openCLcontext->createBuffer(1+(nb_hit*2),sizeof(int),CL_MEM_READ_WRITE);
    this->kernel->setArg(3, OpenCLContext::openCLcontext->getBuffer(buffId[2]));
}
#endif
