#include "PLYShape.h"
#include "Triangle.h"
#include "Thread.h"
#include "AutoLock.h"
#include "OpenCLContext.h"
#include "SmartPointer.h"

#include <math.h>
#include <stdio.h>

#define PLYBOX_RADIUS_FCT 25.0
#define LARGEBOX_RADIUS_FCT 3.0

PLYShape::PLYShape(const char* f,bool smooth,double size,const Mark& mk)
    :Shape(mk),Lockable(),
     size(Treble<double>(1,-1,-1)*size)
{
    smoothNormal=smooth;
    buildFromFile(f);
}

PLYShape::PLYShape(bool smooth,double size,const Mark& mk)
    :Shape(mk),Lockable(),
     size(Treble<double>(1,1,1)*size),box(NULL)
{
    smoothNormal=smooth;
}

PLYShape::~PLYShape()
{
    for(int i=0; i<largeBoxes._count(); i++)
    {
        delete largeBoxes[i]->box;
        delete largeBoxes[i];
    }

    for(int i=0; i<boxes._count(); i++)
    {
        delete boxes[i].box;
    }

#ifdef OpenCL
    for(int i=0; i<4; i++)
        OpenCLContext::openCLcontext->releaseBuffer(hit_buffId[i]);
#endif
}

ObjCollection<Hit> PLYShape::getHit(const ObjCollection<Ray>& r)const
{
    return _getHit(r);
}

Hit PLYShape::_getHit(const Ray& r)const
{
    ObjCollection<Ray> rc;
    rc._add(r);
    return _getHit(rc)[0];
}

ObjCollection<Hit> PLYShape::_getHit(const ObjCollection<Ray>& r)const
{
    const PLYPrimitive** p=smoothNormal?(const PLYPrimitive**)malloc(r._count()*sizeof(const PLYPrimitive*)):NULL;
    const PLYBox** b=smoothNormal?(const PLYBox**)malloc(r._count()*sizeof(const PLYBox*)):NULL;
    ObjCollection<Hit> h=__getHit(r,smoothNormal?&p:NULL,smoothNormal?&b:NULL);

    if(smoothNormal)
    {
        for(int l=0; l<r._count(); l++)
        {
            if(!(h[l].isNull()))
            {
                Vector n=Vector::null;
                double dst=EPSILON;
                CollectionUnion<const PLYPrimitive*> prmUnion=CollectionUnion<const PLYPrimitive*>(2,&b[l]->ht,&b[l]->prm);
                Collection<int> cTab;
                for(int i=0; i<prmUnion._count(); i++)
                {
                    bool flg=false;
                    for(int j=0; !flg&&j<3; j++)
                        for(int k=j; !flg&&k<3; k++)
                            flg|=Point(prmUnion[i]->pt[j])==Point(p[l]->pt[k]);
                    if(flg)
                    {
                        cTab._add(i);
                        dst=MAX(dst,p[l]->b.dist(prmUnion[i]->b));
                    }
                }
                for(int i=0; i<cTab._count(); i++)
                {
                    Vector w=prmUnion[cTab[i]]->n;
                    if(p[l]->n.angle(w)>M_PI/2.0)w=-w;
                    if(p[l]->n.angle(w)<M_PI/4.0)
                        n+=w*SQ(1.0-h[l].getPoint().dist(prmUnion[cTab[i]]->b)/dst);
                }

                h.getTab()[l].setThNormal(n.isNull()?p[l]->n:n.norm());
            }
        }
    }

    if(p!=NULL)free(p);
    if(b!=NULL)free(b);

    return h;
}

ObjCollection<Hit> PLYShape::__getHit(const ObjCollection<Ray>& r,const PLYPrimitive*** p,const PLYBox*** b)const
{
    ObjCollection<Hit> hc(r._count());
    bool flgInter=(box==NULL);
    for(int i=0; i<r._count(); i++)
    {
        hc._add(Hit::null);
        if(!flgInter)flgInter|=box->intersect(r[i]);
    }

    if(flgInter)
    {
        int nbShapes=0;
        SmartTabPointer<int> bCnt=(int*)malloc((1+boxes._count()+2)*sizeof(int));
        bCnt[0]=0;
        for(int i=0; i<largeBoxes._count(); i++)
        {
            flgInter=false;
            for(int j=0; !flgInter&&j<r._count(); j++)
                flgInter|=largeBoxes[i]->box->intersect(r[j]);
            if(flgInter)
            {
                for(int j=0; j<largeBoxes[i]->boxes._count(); j++)
                {
                    int cnt=largeBoxes[i]->boxes[j]->ht._count();
                    flgInter=false;
                    for(int k=0; !flgInter&&k<r._count(); k++)
                        flgInter|=largeBoxes[i]->boxes[j]->box->intersect(r[k]);
                    if(flgInter)nbShapes+=cnt;
                    else cnt*=-1;
                    if(ZSIGN(bCnt[bCnt[0]])!=ZSIGN(cnt))bCnt[++bCnt[0]]=0;
                    bCnt[bCnt[0]]+=cnt;
                }
            }
            else
            {
                if(SIGN(bCnt[bCnt[0]])>0)bCnt[++bCnt[0]]=0;
                bCnt[bCnt[0]]-=largeBoxes[i]->cntHt;
            }
        }

#ifdef OpenCL
        if(nbShapes>this->shapes._count()/this->boxes._count()&&OpenCLContext::openCLQueue.tryEnqueueLock()==0)
        {
            bCnt[bCnt[0]+1]=r._count();
            double *k_r=(double*)malloc(r._count()*2*TREBLE_SIZE*sizeof(double));
            for(int i=0; i<r._count(); i++)
                for(int j=0; j<TREBLE_SIZE; j++)
                    k_r[(i*2*TREBLE_SIZE)+j]=(double)r[i].getPoint().get(j),k_r[(((i*2)+1)*TREBLE_SIZE)+j]=(double)r[i].getVector().get(j);

            OpenCLContext::openCLQueue.waitLock();
            if(r._count()>nb_ray)
            {
                nb_ray=r._count();
                OpenCLContext::openCLcontext->releaseBuffer(hit_buffId[2]);
                hit_buffId[2]=OpenCLContext::openCLcontext->createBuffer(nb_ray*2*TREBLE_SIZE,sizeof(double),CL_MEM_READ_ONLY);
                this->hit_kernel->setArg(2, OpenCLContext::openCLcontext->getBuffer(hit_buffId[2]));
            }
            OpenCLContext::openCLcontext->writeBuffer(hit_buffId[2],r._count()*2*TREBLE_SIZE,sizeof(double),k_r);
            free(k_r);

            _runHitKernel(nbShapes,r,hc,bCnt,p,b);
        }
        else
#endif
        {
            if(nbShapes>0)
            {
                int id=1;
                for(int i=0; i<largeBoxes._count(); i++)
                {
                    if(bCnt[id]<0&&abs(bCnt[id])>=largeBoxes[i]->cntHt)
                    {
                        bCnt[id]+=largeBoxes[i]->cntHt;
                        if(bCnt[id]==0)id++;
                    }
                    else
                    {
                        for(int j=0; j<largeBoxes[i]->boxes._count(); j++)
                        {
                            if(bCnt[id]>0)
                                for(int k=0; k<largeBoxes[i]->boxes[j]->ht._count(); k++)
                                    for(int l=0; l<r._count(); l++)
                                        _addHit(r,hc,l,largeBoxes[i]->boxes[j],k,p,b);
                            bCnt[id]-=largeBoxes[i]->boxes[j]->ht._count()*SIGN(bCnt[id]);
                            if(bCnt[id]==0)id++;
                        }
                    }
                }
            }
        }
    }

    return hc;
}

#ifdef OpenCL
void PLYShape::_runHitKernel(int nbShapes, const ObjCollection<Ray>& r,ObjCollection<Hit>& hc,SmartTabPointer<int> bCnt,const PLYPrimitive*** p,const PLYBox*** b)const
{
    int nb=0;
    bCnt[bCnt[0]+2]=nb_hit;
    OpenCLContext::openCLcontext->writeBuffer(hit_buffId[1],1+bCnt[0]+2,sizeof(int),bCnt.getPointer());
    OpenCLContext::openCLcontext->writeBuffer(hit_buffId[3],1,sizeof(int),&nb);

    this->hit_kernel->runKernel(nbShapes);

    OpenCLContext::openCLcontext->readBuffer(hit_buffId[3],1,sizeof(int),&nb);
    if(nb==0)
    {
        OpenCLContext::openCLcontext->flush();
        OpenCLContext::openCLQueue.unlock();
    }
    else if(nb<=nb_hit)
    {
        SmartTabPointer<int> ind=(int*)malloc((1+(nb*2))*sizeof(int));
        OpenCLContext::openCLcontext->readBuffer(hit_buffId[3],1+(nb*2),sizeof(int),ind);
        OpenCLContext::openCLcontext->flush();
        OpenCLContext::openCLQueue.unlock();

        for(int n=0; n<nb; n++)
        {
            int shapeId=ind[1+(n*2)+1];
            for(int i=0,id=shapeId; id>=0&&i<largeBoxes._count(); i++)
            {
                if(id<largeBoxes[i]->cntHt)
                {
                    for(int j=0; id>=0&&j<largeBoxes[i]->boxes._count(); j++)
                    {
                        if(id<largeBoxes[i]->boxes[j]->ht._count())
                        {
                            for(int k=n; k<nb; k++)
                            {
                                if(shapeId==ind[1+(k*2)+1])
                                {
                                    _addHit(r,hc,ind[1+(k*2)],largeBoxes[i]->boxes[j],id,p,b);
                                    ind[1+(k*2)+1]=-1;
                                }
                            }
                        }
                        id-=largeBoxes[i]->boxes[j]->ht._count();
                    }
                }
                else id-=largeBoxes[i]->cntHt;
            }
        }
    }
    else
    {
        nb_hit=nb;
        OpenCLContext::openCLcontext->releaseBuffer(hit_buffId[3]);
        hit_buffId[3]=OpenCLContext::openCLcontext->createBuffer(1+(nb_hit*2),sizeof(int),CL_MEM_READ_WRITE);
        this->hit_kernel->setArg(3, OpenCLContext::openCLcontext->getBuffer(hit_buffId[3]));
        _runHitKernel(nbShapes,r,hc,bCnt,p,b);
    }
}
#endif

void PLYShape::_addHit(const ObjCollection<Ray>& r,ObjCollection<Hit>& hc,int k,const PLYBox* box,int id,const PLYPrimitive*** p,const PLYBox*** b)const
{
    Hit h=Triangle::getTriangleHit(r[k],this,box->ht[id]->pt);
    Hit ht=tamperHit(h,r[k]);
    if(!(ht.isNull()))
    {
        if((hc[k].isNull())||(r[k].getPoint().dist(ht.getPoint())<r[k].getPoint().dist(hc[k].getPoint())))
        {
            ht.setId(id);
            if(p!=NULL)(*p)[k]=box->ht[id];
            if(b!=NULL)(*b)[k]=box;
            hc.getTab()[k]=ht;
        }
    }
}

ObjCollection<Hit> PLYShape::getIntersect(const ObjCollection<Ray>& r)const
{
    return __getHit(r);
}

bool PLYShape::isInside(const Point& p,double e)const
{
    if(box==NULL||box->isInside(p,e))
    {
        int n=0;
        Ray r=Ray(p,getMark().getOrig().getVectorTo(p).norm());
        Hit h=Hit::null;
        do
        {
            h=_getHit(r);

            if(!h.isNull())
            {
                r=Ray(h.getPoint()+(r.getVector()*EPSILON),r.getVector());
                n++;
            }
        }
        while(!h.isNull());

        return (n%2)!=0;
    }
    return false;
}

Point PLYShape::PLYtoRef(const Point& pt)const
{
    return getMark().toRef(Point(pt*this->size));
}

void PLYShape::buildBoxes(bool flgBox)
{
    shapes.trim();

    if(flgBox)
    {
        Point bounds[2];
        bool flg=false;
        for(int i=0; i<shapes._count(); i++)
        {
            for(int j=0; j<3; j++)
            {
                const Point& p=shapes.get(i).pt[j];
                if(flg)
                {
                    bounds[0]=min(bounds[0],p);
                    bounds[1]=max(bounds[1],p);
                }
                else
                {
                    for(int k=0; k<2; k++)bounds[k]=p;
                    flg=true;
                }
            }
        }

        fprintf(stdout,"Bounds :\n");
        TUPLE_IDX(i,TREBLE_SIZE)
        fprintf(stdout,"%c : [%lf ; %lf]\n",'x'+i,bounds[0][i],bounds[1][i]);
        fflush(stdout);

        Point p0=Point::barycenter(bounds[0],bounds[1]);
        double d=-1.0;
        for(int i=0; i<shapes._count(); i++)
            for(int j=0; j<3; j++)
                d=MAX(d,p0.dist(shapes.get(i).pt[j]));
        if(d>0)setBox(new Sphere(d+EPSILON,Mark(p0)));
    }

    for(int i=0; i<shapes._count(); i++)
    {
        if((i+1)%1000==0||i+1==shapes._count())
        {
            fprintf(stdout,"\rBuilding boxes : %d/%d (%.2lf%%)",i+1,shapes._count(),((double)(i+1)/shapes._count())*100.);
            fflush(stdout);
        }
        bool flg=false;
        for(int j=boxes._count()-1; !flg&&j>=0; j--)
        {
            flg=boxes[j].ht._count()<shapes._count()/1000;
            for(int k=0; flg&&k<3; k++)
                flg&=boxes[j].box->isInside(Point(shapes[i].pt[k]));
            if(flg)boxes.getTab()[j].ht._add(&shapes[i]);
        }
        if(!flg)
        {
            double d=0.0;
            for(int j=0; j<3; j++)
                d=MAX(d,Point(shapes[i].pt[j]).dist(shapes[i].b));
            addBox(shapes[i].b,d*PLYBOX_RADIUS_FCT);
            boxes.getTab()[boxes._count()-1].ht._add(&shapes[i]);
        }
    }
    boxes.trim();

    fprintf(stdout,"\nBoxes : %d\n",boxes._count());
    fflush(stdout);

    for(int i=0; i<boxes._count(); i++)
    {
        boxes.getTab()[i].ht.trim();

        Point bounds[2];
        bool flg=false;
        for(int j=0; j<boxes[i].ht._count(); j++)
        {
            for(int k=0; k<3; k++)
            {
                const Point& p=boxes[i].ht[j]->pt[k];
                if(flg)
                {
                    bounds[0]=min(bounds[0],p);
                    bounds[1]=max(bounds[1],p);
                }
                else
                {
                    for(int l=0; l<2; l++)bounds[l]=p;
                    flg=true;
                }
            }
        }
        boxes.getTab()[i].box->setMark(Mark(Point::barycenter(bounds[0],bounds[1])));

        double dMax=-1.0;
        for(int j=0; j<boxes[i].ht._count(); j++)
            for(int k=0; k<3; k++)
                dMax=MAX(dMax,boxes[i].box->getMark().getOrig().dist(boxes[i].ht[j]->pt[k]));
        boxes.getTab()[i].box->setRadius(dMax+EPSILON);

        double dMin=-1.0;
        PLYLargeBox* lb=NULL;

        for(int j=0; j<largeBoxes._count(); j++)
        {
            double d=largeBoxes[j]->box->getMark().getOrig().dist(boxes[i].box->getMark().getOrig())+boxes[i].box->getRadius();
            if(d<largeBoxes[j]->box->getRadius()&&(dMin<0||d<dMin))
            {
                dMin=d;
                lb=largeBoxes.getTab()[j];
            }
        }

        if(lb==NULL)
        {
            lb=new PLYLargeBox();
            lb->box=new Sphere(boxes[i].box->getRadius()*LARGEBOX_RADIUS_FCT,boxes[i].box->getMark());
            largeBoxes._add(lb);
        }
        lb->boxes._add(&(boxes[i]));
    }

    for(int i=0; i<largeBoxes._count(); i++)
    {
        double dMax=-1.0;
        largeBoxes[i]->cntHt=0;
        for(int j=0; j<largeBoxes[i]->boxes._count(); j++)
        {
            dMax=MAX(dMax,largeBoxes[i]->box->getMark().getOrig().dist(largeBoxes[i]->boxes[j]->box->getMark().getOrig())+largeBoxes[i]->boxes[j]->box->getRadius());
            largeBoxes[i]->cntHt+=largeBoxes[i]->boxes[j]->ht._count();
        }
        largeBoxes.getTab()[i]->box->setRadius(dMax+EPSILON);
    }

    fprintf(stdout,"Large boxes : %d\n",largeBoxes._count());
    fflush(stdout);

    if(smoothNormal)
    {
#ifdef OpenCL
        int cnt=this->shapes._count(),maxHt=0;
        for(int i=0; i<boxes._count(); i++)
            maxHt=MAX(maxHt,this->boxes[i].ht._count());
        adj_buffId[0]=OpenCLContext::openCLcontext->createBuffer(cnt*3*TREBLE_SIZE,sizeof(double),CL_MEM_READ_ONLY);
        adj_buffId[1]=OpenCLContext::openCLcontext->createBuffer(1,sizeof(int),CL_MEM_READ_ONLY);
        adj_buffId[2]=OpenCLContext::openCLcontext->createBuffer(maxHt*3*TREBLE_SIZE,sizeof(double),CL_MEM_READ_ONLY);
        adj_buffId[3]=OpenCLContext::openCLcontext->createBuffer(1,sizeof(int),CL_MEM_READ_ONLY);
        adj_buffId[4]=OpenCLContext::openCLcontext->createBuffer(TREBLE_SIZE+1,sizeof(double),CL_MEM_READ_ONLY);
        adj_buffId[5]=OpenCLContext::openCLcontext->createBuffer(cnt,sizeof(int),CL_MEM_READ_WRITE);

        double *pt=(double*)malloc(cnt*3*TREBLE_SIZE*sizeof(double));
        for(int i=0; i<cnt; i++)
            for(int j=0; j<3; j++)
                for(int k=0; k<TREBLE_SIZE; k++)
                    pt[(((i*3)+j)*TREBLE_SIZE)+k]=(double)this->shapes[i].pt[j].get(k);
        OpenCLContext::openCLcontext->writeBuffer(adj_buffId[0],cnt*3*TREBLE_SIZE,sizeof(double),pt);
        OpenCLContext::openCLcontext->writeBuffer(adj_buffId[1],1,sizeof(int),&cnt);
        free(pt);

        this->adj_kernel=new OpenCLKernel("adj_primitive", "#pragma OPENCL EXTENSION cl_khr_fp64 : enable\n\
__kernel void adj_primitive(\
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

        for(int i=0; i<6; i++)
            this->adj_kernel->setArg(i, OpenCLContext::openCLcontext->getBuffer(adj_buffId[i]));
#endif

        nb_box=0;
        Thread::run(boxThread,this);
        fprintf(stdout,"\n");
        fflush(stdout);

#ifdef OpenCL
        for(int i=0; i<6; i++)
            OpenCLContext::openCLcontext->releaseBuffer(adj_buffId[i]);
#endif
    }

#ifdef OpenCL
    nb_ray=1,nb_hit=0;
    hit_buffId[0]=OpenCLContext::openCLcontext->createBuffer(shapes._count()*3*TREBLE_SIZE,sizeof(double),CL_MEM_READ_ONLY);
    hit_buffId[1]=OpenCLContext::openCLcontext->createBuffer(1+boxes._count()+2,sizeof(int),CL_MEM_READ_ONLY);
    hit_buffId[2]=OpenCLContext::openCLcontext->createBuffer(nb_ray*2*TREBLE_SIZE,sizeof(double),CL_MEM_READ_ONLY);
    hit_buffId[3]=OpenCLContext::openCLcontext->createBuffer(1+(nb_hit*2),sizeof(int),CL_MEM_READ_WRITE);

    int nbShapes=0;
    double *pt=(double*)malloc(shapes._count()*3*TREBLE_SIZE*sizeof(double));
    for(int i=0; i<largeBoxes._count(); i++)
        for(int j=0; j<largeBoxes[i]->boxes._count(); j++)
            for(int k=0; k<largeBoxes[i]->boxes[j]->ht._count(); k++,nbShapes++)
                for(int l=0; l<3; l++)
                    for(int m=0; m<TREBLE_SIZE; m++)
                        pt[(((nbShapes*3)+l)*TREBLE_SIZE)+m]=(double)largeBoxes[i]->boxes[j]->ht[k]->pt[l].get(m);
    OpenCLContext::openCLcontext->writeBuffer(hit_buffId[0],shapes._count()*3*TREBLE_SIZE,sizeof(double),pt);
    free(pt);

    this->hit_kernel=new OpenCLKernel("primitive_hit", "#pragma OPENCL EXTENSION cl_khr_fp64 : enable\n\
__kernel void primitive_hit(\
    __global const double *prm,\
    __global const int *cnt,\
    __global const double *r,\
    __global int *k)\
{\
    int id=get_global_id(0),sid=0;\
    for(int i=1;sid<=id&&i<=cnt[0];sid+=abs(cnt[i++]))\
        if(cnt[i]<0)id+=abs(cnt[i]);\
    if(id>=sid)return;\
    double3 t_o=vload3(id*3,prm);\
    double3 t_v1=vload3((id*3)+1,prm)-t_o;\
    double3 t_v2=vload3((id*3)+2,prm)-t_o;\
    for(int ir=0;ir<cnt[cnt[0]+1];ir++)\
    {\
        double3 t_pt=vload3(ir*2,r);\
        double3 t_vct=vload3(ir*2+1,r);\
        double d=dot(cross(t_v2,t_v1),t_vct);\
        double3 t_w=t_pt-t_o;\
        double a=dot(cross(t_v2,t_w),t_vct)/d;\
        double b=dot(cross(t_w,t_v1),t_vct)/d;\
        if(a>=0.0 && a<=1.0 && b>=0.0 && b<=1.0 && (a+b)<=1.0)\
            if(dot(cross(t_v1,t_v2),t_w)/d>0.0)\
            {\
                int ik=atomic_inc(k);\
                if(ik<cnt[cnt[0]+2])\
                {\
                    k[1+(ik*2)]=ir;\
                    k[1+(ik*2)+1]=id;\
                }\
            }\
    }\
}");

    for(int i=0; i<4; i++)
        this->hit_kernel->setArg(i, OpenCLContext::openCLcontext->getBuffer(hit_buffId[i]));
#endif
}

void PLYShape::addPrimitive(const Point& a,const Point& b,const Point& c)
{
    PLYPrimitive p;
    p.pt[0]=a,p.pt[1]=b,p.pt[2]=c;
    p.b=Point::barycenter(p.pt[0],p.pt[1],p.pt[2]);
    p.n=Triangle::getTriangleNormal(p.pt).norm();
    shapes._add(p);
}

void PLYShape::addBox(const Point& pt,double r)
{
    PLYBox b;
    b.box=new Sphere(r,Mark(pt));
    boxes._add(b);
}

bool PLYShape::getNextBox(int *n)
{
    AutoLock autolock(this);
    (*n)=nb_box++;
    bool r=((*n)<boxes._count());
    if(r&&(nb_box%10==0||nb_box==boxes._count()))
    {
        fprintf(stdout,"\rFilling boxes : %d/%d (%.2lf%%)",nb_box,boxes._count(),((double)nb_box/boxes._count())*100.);
        fflush(stdout);
    }
    return r;
}

void* boxThread(void* d)
{
    PLYShape* s=(PLYShape*)d;
    int i=0;
    while(s->getNextBox(&i))
    {
#ifdef OpenCL
        if(OpenCLContext::openCLQueue.tryEnqueueLock()==0)
        {
            double *pt=(double*)malloc(s->boxes[i].ht._count()*3*TREBLE_SIZE*sizeof(double));
            for(int j=0; j<s->boxes[i].ht._count(); j++)
                for(int k=0; k<3; k++)
                    for(int l=0; l<TREBLE_SIZE; l++)
                        pt[(((j*3)+k)*TREBLE_SIZE)+l]=(double)s->boxes[i].ht[j]->pt[k].get(l);
            int cnt=s->boxes[i].ht._count(),nb=0;
            double bx[TREBLE_SIZE+1];
            for(int j=0; j<TREBLE_SIZE; j++)
                bx[j]=(double)s->boxes[i].box->getMark().getOrig().get(j);
            bx[TREBLE_SIZE]=(double)s->boxes[i].box->getRadius();

            OpenCLContext::openCLQueue.waitLock();
            OpenCLContext::openCLcontext->writeBuffer(s->adj_buffId[2],s->boxes[i].ht._count()*3*TREBLE_SIZE,sizeof(double),pt);
            OpenCLContext::openCLcontext->writeBuffer(s->adj_buffId[3],1,sizeof(int),&cnt);
            OpenCLContext::openCLcontext->writeBuffer(s->adj_buffId[4],TREBLE_SIZE+1,sizeof(double),bx);
            OpenCLContext::openCLcontext->writeBuffer(s->adj_buffId[5],1,sizeof(int),&nb);
            free(pt);

            s->adj_kernel->runKernel(s->shapes._count());

            OpenCLContext::openCLcontext->readBuffer(s->adj_buffId[5],1,sizeof(int),&nb);
            if(nb>0)
            {
                SmartTabPointer<int> ind=(int*)malloc((1+nb)*sizeof(int));
                OpenCLContext::openCLcontext->readBuffer(s->adj_buffId[5],1+nb,sizeof(int),ind);

                OpenCLContext::openCLcontext->flush();
                OpenCLContext::openCLQueue.unlock();

                for(int j=1; j<=nb; j++)
                    s->boxes.getTab()[i].prm._add(&s->shapes[ind[j]]);
            }
            else
            {
                OpenCLContext::openCLcontext->flush();
                OpenCLContext::openCLQueue.unlock();
            }
        }
        else
#endif
        {
            for(int j=0; j<s->shapes._count(); j++)
            {
                bool flg=false;
                for(int k=0; !flg&&k<3; k++)
                    flg|=s->boxes[i].box->isInside(s->shapes[j].pt[k]);
                if(flg)
                {
                    int nb=0;
                    for(int k=0; nb<3&&k<s->boxes[i].ht._count(); k++)
                    {
                        int n=0;
                        for(int l=0; l<3; l++)
                            for(int m=l; m<3; m++)
                                if(s->shapes[j].pt[l]==s->boxes[i].ht[k]->pt[m])n++;
                        nb=MAX(nb,n);
                    }
                    if(nb>0&&nb<3)
                        s->boxes.getTab()[i].prm._add(&s->shapes[j]);
                }
            }
        }

        s->boxes.getTab()[i].prm.trim();
    }
    return NULL;
}

void PLYShape::buildFromFile(const char* filename)
{
    fprintf(stdout,"Reading PLY file %s\n",filename);
    fflush(stdout);

    FILE* f=NULL;
    if((f=fopen(filename,"r"))!=NULL)
    {
        char buf[BUFSIZ];
        if(fgets(buf,BUFSIZ,f)!=NULL)
        {
            if(strcmp(buf,"ply\n")==0)
            {
                if(fgets(buf,BUFSIZ,f)!=NULL)
                {
                    if(strncmp(buf,"format",strlen("format"))==0)
                    {
                        char fmt[BUFSIZ];
                        sscanf(buf+strlen("format"),"%s",fmt);

                        fgets(buf,BUFSIZ,f);//comment

                        if(fgets(buf,BUFSIZ,f)!=NULL)
                        {
                            int nb_vertex=0;
                            sscanf(buf+strlen("element vertex"),"%d",&nb_vertex);
                            fprintf(stdout,"Vertex : %d\n",nb_vertex);
                            fflush(stdout);

                            do
                            {
                                fgets(buf,BUFSIZ,f);
                            }
                            while(strncmp(buf,"property",strlen("property"))==0);

                            int nb_face=0;
                            sscanf(buf+strlen("element face"),"%d",&nb_face);
                            fprintf(stdout,"Face : %d\n",nb_face);
                            fflush(stdout);

                            do
                            {
                                fgets(buf,BUFSIZ,f);
                            }
                            while(strncmp(buf,"property",strlen("property"))==0);

                            if(strcmp(fmt,"ascii")!=0)
                            {
                                fprintf(stderr,"Invalid file format : %s\n",fmt);
                                fflush(stderr);
                                exit(1);
                            }

                            Point pt;
                            ObjCollection<Point> vtx;
                            while(nb_vertex-->0)
                            {
                                fgets(buf,BUFSIZ,f);
                                sscanf(buf,"%lf %lf %lf",&(pt.getTab()[0]),&(pt.getTab()[1]),&(pt.getTab()[2]));
                                vtx._add(PLYtoRef(pt));
                            }

                            vtx.trim();

                            while(nb_face-->0)
                            {
                                int n=0;
                                if(strcmp(fmt,"ascii")==0)
                                {
                                    fgets(buf,BUFSIZ,f);
                                    sscanf(buf,"%d",&n);
                                }

                                if(n==3)
                                {
                                    int a=0,b=0,c=0;
                                    sscanf(buf,"%d %d %d %d",&n,&a,&b,&c);

                                    addPrimitive(vtx[a],vtx[b],vtx[c]);
                                }
                                else if(n==4)
                                {
                                    int a=0,b=0,c=0,d=0;
                                    sscanf(buf,"%d %d %d %d %d",&n,&a,&b,&c,&d);

                                    addPrimitive(vtx[a],vtx[b],vtx[c]);
                                    addPrimitive(vtx[a],vtx[d],vtx[c]);
                                }
                                else
                                {
                                    fprintf(stderr,"Invalid face description\n");
                                    fflush(stderr);
                                    exit(1);
                                }
                            }

                            buildBoxes();
                        }
                    }
                }
            }
            else
            {
                fprintf(stderr,"Invalid file type\n");
                fflush(stderr);
                exit(1);
            }
        }
        else
        {
            fprintf(stderr,"Error reading file\n");
            fflush(stderr);
            exit(1);
        }

        fclose(f);
    }
    else
    {
        fprintf(stderr,"Error reading file %s\n",filename);
        fflush(stderr);
        exit(1);
    }
}
