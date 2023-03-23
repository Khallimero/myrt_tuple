#include "ConcurrentOpenCLKernelCollection.h"

#ifdef OpenCL
ConcurrentOpenCLKernelCollection::ConcurrentOpenCLKernelCollection()
    :BinaryTree<ConcurrentOpenCLKernel>()
{
    setAutoDelete(true);
}

ConcurrentOpenCLKernelCollection::~ConcurrentOpenCLKernelCollection() {}

ConcurrentOpenCLKernel* ConcurrentOpenCLKernelCollection::findKernel()const
{
    ConcurrentOpenCLKernel kernelId;
    return cfind(&kernelId);
}

int ConcurrentOpenCLKernelCollection::compareItems(const ConcurrentOpenCLKernel* k1,const ConcurrentOpenCLKernel* k2)const
{
    return k1->getThreadId()-k2->getThreadId();
}
#endif
