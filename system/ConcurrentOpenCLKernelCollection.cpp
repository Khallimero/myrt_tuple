#include "ConcurrentOpenCLKernelCollection.h"

#include "AutoLock.h"

#ifdef OpenCL
ConcurrentOpenCLKernelCollection::ConcurrentOpenCLKernelCollection()
    :BinaryTree<ConcurrentOpenCLKernel>()
{
    setAutoDelete(true);
}

ConcurrentOpenCLKernelCollection::~ConcurrentOpenCLKernelCollection() {}

ConcurrentOpenCLKernel* ConcurrentOpenCLKernelCollection::findKernel()
{
    AutoLock autolock(this);

    ConcurrentOpenCLKernel kernelId;
    ConcurrentOpenCLKernel* kernel=cfind(&kernelId);
    if(kernel==NULL)
    {
        kernel=createKernel();
        insert(kernel);
    }

    return kernel;
}

int ConcurrentOpenCLKernelCollection::compareItems(const ConcurrentOpenCLKernel* k1,const ConcurrentOpenCLKernel* k2)const
{
    return k1->getThreadId()-k2->getThreadId();
}
#endif
