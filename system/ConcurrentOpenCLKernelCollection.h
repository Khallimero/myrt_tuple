#pragma once

#ifdef OpenCL
#include "ConcurrentOpenCLKernel.h"
#include "BinaryTree.h"

class ConcurrentOpenCLKernelCollection : public BinaryTree<ConcurrentOpenCLKernel>
{
public:
    ConcurrentOpenCLKernelCollection();
    virtual ~ConcurrentOpenCLKernelCollection();

public:
    ConcurrentOpenCLKernel* findKernel()const;

public:
    virtual int compareItems(const ConcurrentOpenCLKernel* k1,const ConcurrentOpenCLKernel* k2)const;
};
#endif
