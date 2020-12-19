#include "PLYShape.h"
#include "Triangle.h"
#include "Thread.h"
#include "AutoLock.h"
#include "OpenCLContext.h"

#include <math.h>
#include <stdio.h>

#define PLYBOX_RADIUS_FCT 20.0
#define LARGEBOX_RADIUS_FCT 5.0

PLYShape::PLYShape(const char* f,double size,const Mark& mk)
    :Shape(mk),Lockable(),
     size(Treble<double>(1,-1,-1)*size),smoothNormal(true)
{
    buildFromFile(f);
}

PLYShape::PLYShape(double size,const Mark& mk)
    :Shape(mk),Lockable(),
     size(Treble<double>(1,1,1)*size),smoothNormal(true),box(NULL)
{
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
}

Hit PLYShape::_getHit(const Ray& r)const
{
    Hit h=Hit::null;

    const PLYPrimitive* p=NULL;
    const PLYBox* b=NULL;
#ifdef OpenCL
    int buffId=-1;
    h=__getHit(r,&p,&b,&buffId);
#else
    h=__getHit(r,&p,&b);
#endif

    if(!h.isNull()&&smoothNormal)
    {
        Vector n=Vector::null;
        double dst=EPSILON;
        CollectionUnion<const PLYPrimitive*> prmUnion=CollectionUnion<const PLYPrimitive*>(2,&b->ht,&b->prm);

#ifdef OpenCL
        if(OpenCLContext::openCLQueue.tryEnqueueLock()==0)
        {
            OpenCLContext::openCLQueue.waitLock();
            int id=h.getId();
            int cnt=prmUnion._count();

            this->nrm_kernel->setArg(0, OpenCLContext::openCLcontext->getBuffer(box_buffId[buffId]));
            OpenCLContext::openCLcontext->writeBuffer(nrm_buffId[0],1,sizeof(int),&cnt);
            OpenCLContext::openCLcontext->writeBuffer(nrm_buffId[1],1,sizeof(int),&id);

            this->nrm_kernel->runKernel(cnt);

            int nb=-1;
            OpenCLContext::openCLcontext->readBuffer(nrm_buffId[2],1,sizeof(int),&nb);
            if(nb>0)
            {
                int* ind=(int*)malloc((1+nb)*sizeof(int));
                OpenCLContext::openCLcontext->readBuffer(nrm_buffId[2],1+nb,sizeof(int),ind);
                OpenCLContext::openCLcontext->flush();
                OpenCLContext::openCLQueue.unlock();

                for(int i=1; i<=nb; i++)
                    dst=MAX(dst,p->b.dist(prmUnion[ind[i]]->b));
                for(int i=1; i<=nb; i++)
                {
                    Vector w=prmUnion[ind[i]]->n;
                    if(p->n.angle(w)>M_PI/2.0)w=-w;
                    if(p->n.angle(w)<M_PI/4.0)
                        n+=w*SQ(1.0-h.getPoint().dist(prmUnion[ind[i]]->b)/dst);
                }

                free(ind);
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
            Collection<int> cTab;
            for(int i=0; i<prmUnion._count(); i++)
            {
                bool flg=false;
                for(int j=0; !flg&&j<3; j++)
                    for(int k=0; !flg&&k<3; k++)
                        if(Point(prmUnion[i]->pt[j])==Point(p->pt[k]))flg=true;
                if(flg)
                {
                    cTab._add(i);
                    dst=MAX(dst,p->b.dist(prmUnion[i]->b));
                }
            }
            for(int i=0; i<cTab._count(); i++)
            {
                Vector w=prmUnion[cTab[i]]->n;
                if(p->n.angle(w)>M_PI/2.0)w=-w;
                if(p->n.angle(w)<M_PI/4.0)
                    n+=w*SQ(1.0-h.getPoint().dist(prmUnion[cTab[i]]->b)/dst);
            }
        }

        h.setThNormal(n.isNull()?p->n:n.norm());
    }

    return h;
}

Hit PLYShape::__getHit(const Ray& r,const PLYPrimitive** p,const PLYBox** b,int *bufferId)const
{
    Hit h=Hit::null;

    if(box==NULL||box->intersect(r))
    {
        double dMin=-1.0;
#ifdef OpenCL
        int buffId=0;
        double k_r[2*TREBLE_SIZE];
        for(int i=0; i<TREBLE_SIZE; i++)
            k_r[i]=(double)r.getPoint().get(i),k_r[TREBLE_SIZE+i]=(double)r.getVector().get(i);
#endif
        for(int i=0; i<largeBoxes._count(); i++)
        {
            if(largeBoxes[i]->box->intersect(r))
            {
                for(int j=0; j<largeBoxes[i]->boxes._count(); j++)
                {
                    if(largeBoxes[i]->boxes[j]->box->intersect(r))
                    {
#ifdef OpenCL
                        if(OpenCLContext::openCLQueue.tryEnqueueLock()==0)
                        {
                            OpenCLContext::openCLQueue.waitLock();
                            int cnt=largeBoxes[i]->boxes[j]->ht._count();
                            this->hit_kernel->setArg(0, OpenCLContext::openCLcontext->getBuffer(box_buffId[buffId+j]));
                            OpenCLContext::openCLcontext->writeBuffer(hit_buffId[0],1,sizeof(int),&cnt);
                            OpenCLContext::openCLcontext->writeBuffer(hit_buffId[1],2*TREBLE_SIZE,sizeof(double),k_r);

                            this->hit_kernel->runKernel(cnt);

                            int nb=-1;
                            OpenCLContext::openCLcontext->readBuffer(hit_buffId[2],1,sizeof(int),&nb);
                            if(nb>0)
                            {
                                int* ind=(int*)malloc((1+nb)*sizeof(int));
                                OpenCLContext::openCLcontext->readBuffer(hit_buffId[2],1+nb,sizeof(int),ind);
                                OpenCLContext::openCLcontext->flush();
                                OpenCLContext::openCLQueue.unlock();

                                for(int k=1; k<=nb; k++)
                                {
                                    Hit ht=Triangle::getTriangleHit(r,this,largeBoxes[i]->boxes[j]->ht[ind[k]]->pt);
                                    if(!(ht.isNull()))
                                    {
                                        double d=r.getPoint().dist(ht.getPoint());
                                        if((h.isNull())||(d<dMin))
                                        {
                                            ht.setId(ind[k]);
                                            if(p!=NULL)*p=largeBoxes[i]->boxes[j]->ht[ind[k]];
                                            if(b!=NULL)*b=largeBoxes[i]->boxes[j];
                                            if(bufferId!=NULL)*bufferId=buffId+j;
                                            dMin=d,h=ht;
                                        }
                                    }
                                }
                                free(ind);
                            }
                            else
                            {
                                OpenCLContext::openCLcontext->flush();
                                OpenCLContext::openCLQueue.unlock();
                            }
                        }
                        else
#endif
                            for(int k=0; k<largeBoxes[i]->boxes[j]->ht._count(); k++)
                            {
                                Hit ht=Triangle::getTriangleHit(r,this,largeBoxes[i]->boxes[j]->ht[k]->pt);
                                if(!(ht.isNull()))
                                {
                                    double d=r.getPoint().dist(ht.getPoint());
                                    if((h.isNull())||(d<dMin))
                                    {
                                        ht.setId(k);
                                        if(p!=NULL)*p=largeBoxes[i]->boxes[j]->ht[k];
                                        if(b!=NULL)*b=largeBoxes[i]->boxes[j];
#ifdef OpenCL
                                        if(bufferId!=NULL)*bufferId=buffId+j;
#endif
                                        dMin=d,h=ht;
                                    }
                                }
                            }
                    }
                }
            }
#ifdef OpenCL
            buffId+=largeBoxes[i]->boxes._count();
#endif
        }
    }

    return h;
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
            h=__getHit(r);

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
            flg=true;
            for(int k=0; flg&&k<3; k++)
                if(!boxes[j].box->isInside(Point(shapes[i].pt[k])))flg=false;
        }
        if(!flg)
        {
            double d=0.0;
            for(int j=0; j<3; j++)
                d=MAX(d,Point(shapes[i].pt[j]).dist(shapes[i].b));
            addBox(shapes[i].b,d*PLYBOX_RADIUS_FCT);
        }
    }
    boxes.trim();

    fprintf(stdout,"\nBoxes : %d\n",boxes._count());
    fflush(stdout);

    for(int i=0; i<boxes._count(); i++)
    {
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
        for(int j=0; j<largeBoxes[i]->boxes._count(); j++)
            dMax=MAX(dMax,largeBoxes[i]->box->getMark().getOrig().dist(largeBoxes[i]->boxes[j]->box->getMark().getOrig())+largeBoxes[i]->boxes[j]->box->getRadius());
        largeBoxes.getTab()[i]->box->setRadius(dMax+EPSILON);
    }

    fprintf(stdout,"Large boxes : %d\n",largeBoxes._count());
    fflush(stdout);

    nb_box=0;
    Thread::run(boxThread,this);
    fprintf(stdout,"\n");
    fflush(stdout);

#ifdef OpenCL
    int maxHt=0,maxPrm=0;
    for(int i=0; i<largeBoxes._count(); i++)
    {
        for(int j=0; j<largeBoxes[i]->boxes._count(); j++)
        {
            int cntHt=largeBoxes[i]->boxes[j]->ht._count();
            int cntPrm=largeBoxes[i]->boxes[j]->prm._count();
            double *pt=(double*)malloc((cntHt+cntPrm)*3*TREBLE_SIZE*sizeof(double));
            for(int k=0; k<cntHt; k++)
                for(int l=0; l<3; l++)
                    for(int m=0; m<TREBLE_SIZE; m++)
                        pt[(((k*3)+l)*TREBLE_SIZE)+m]=(double)largeBoxes[i]->boxes[j]->ht[k]->pt[l].get(m);
            for(int k=0; k<cntPrm; k++)
                for(int l=0; l<3; l++)
                    for(int m=0; m<TREBLE_SIZE; m++)
                        pt[((((cntHt+k)*3)+l)*TREBLE_SIZE)+m]=(double)largeBoxes[i]->boxes[j]->prm[k]->pt[l].get(m);
            int buffId=OpenCLContext::openCLcontext->createBuffer((cntHt+cntPrm)*3*TREBLE_SIZE,sizeof(double),CL_MEM_READ_ONLY);
            OpenCLContext::openCLcontext->writeBuffer(buffId,(cntHt+cntPrm)*3*TREBLE_SIZE,sizeof(double),pt);
            box_buffId._add(buffId);
            free(pt);

            maxHt=MAX(maxHt,cntHt);
            maxPrm=MAX(maxPrm,cntHt+cntPrm);
        }
    }

    hit_buffId[0]=OpenCLContext::openCLcontext->createBuffer(1,sizeof(int),CL_MEM_READ_ONLY);
    hit_buffId[1]=OpenCLContext::openCLcontext->createBuffer(2*TREBLE_SIZE,sizeof(double),CL_MEM_READ_ONLY);
    hit_buffId[2]=OpenCLContext::openCLcontext->createBuffer(1+maxHt,sizeof(int),CL_MEM_READ_WRITE);
    nrm_buffId[0]=OpenCLContext::openCLcontext->createBuffer(1,sizeof(int),CL_MEM_READ_ONLY);
    nrm_buffId[1]=OpenCLContext::openCLcontext->createBuffer(1,sizeof(int),CL_MEM_READ_ONLY);
    nrm_buffId[2]=OpenCLContext::openCLcontext->createBuffer(1+maxPrm,sizeof(int),CL_MEM_READ_WRITE);

    this->hit_kernel=new OpenCLKernel("primitive_hit", "#pragma OPENCL EXTENSION cl_khr_fp64 : enable\n\
__kernel void primitive_hit(\
    __global const double *prm,\
    __global const int *cnt,\
    __global const double *r,\
    __global int *k)\
{\
    int id=get_global_id(0);\
    if(id==0)*k=0;\
    barrier(CLK_GLOBAL_MEM_FENCE);\
    if(id>=(*cnt))return;\
    double3 t_pt=vload3(0,r);\
    double3 t_vct=vload3(1,r);\
    double3 t_o=vload3(id*3,prm);\
    double3 t_v1=vload3((id*3)+1,prm)-t_o;\
    double3 t_v2=vload3((id*3)+2,prm)-t_o;\
    double d=dot(cross(t_v2,t_v1),t_vct);\
    double3 t_w=t_pt-t_o;\
    double a=dot(cross(t_v2,t_w),t_vct)/d;\
    double b=dot(cross(t_w,t_v1),t_vct)/d;\
    if(a>=0.0 && a<=1.0 && b>=0.0 && b<=1.0 && (a+b)<=1.0)\
        if(dot(cross(t_v1,t_v2),t_w)/d>0.0)\
            k[atomic_inc(k)+1]=id;\
}");

    for(int i=0; i<3; i++)
        this->hit_kernel->setArg(i+1, OpenCLContext::openCLcontext->getBuffer(hit_buffId[i]));

    this->nrm_kernel=new OpenCLKernel("smooth_normal","#pragma OPENCL EXTENSION cl_khr_fp64 : enable\n\
__kernel void smooth_normal(\
    __global const double *prm,\
    __global const int *cnt,\
    __global const int *ht,\
    __global int* k)\
{\
    int id=get_global_id(0);\
    if(id==0)*k=0;\
    barrier(CLK_GLOBAL_MEM_FENCE);\
    if(id>=(*cnt))return;\
    for(int i=0;i<3;i++)\
        for(int j=0;j<3;j++)\
            if(length(vload3((id*3)+i,prm)-vload3(((*ht)*3)+j,prm))==0)\
            {\
                k[atomic_inc(k)+1]=id;\
                return;\
            }\
}");

    for(int i=0; i<3; i++)
        this->nrm_kernel->setArg(i+1, OpenCLContext::openCLcontext->getBuffer(nrm_buffId[i]));
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
        for(int k=0; k<s->shapes._count(); k++)
        {
            int n=0;
            for(int j=0; j<3; j++)
                if(s->boxes[i].box->isInside(Point(s->shapes[k].pt[j])))n++;
            if(n==3)s->boxes.getTab()[i].ht._add(&s->shapes[k]);
            else if(n>0)s->boxes.getTab()[i].prm._add(&s->shapes[k]);
        }

        s->boxes.getTab()[i].prm.trim();
        s->boxes.getTab()[i].ht.trim();
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

                            ObjCollection<Point> vtx;

                            if(strcmp(fmt,"ascii")!=0)
                            {
                                fprintf(stderr,"Invalid file format : %s\n",fmt);
                                fflush(stderr);
                                exit(1);
                            }

                            Point pt;
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
