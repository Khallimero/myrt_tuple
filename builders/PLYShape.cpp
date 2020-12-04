#include "PLYShape.h"
#include "Triangle.h"
#include "Thread.h"
#include "AutoLock.h"

#include <math.h>
#include <stdio.h>

#define PLYBOX_RADIUS_FCT 10.0
#define LARGEBOX_RADIUS_FCT 5.0

PLYShape::PLYShape(const char* f,double size,const Mark& mk)
    :Shape(mk),Lockable(),
     size(Treble<double>(1,-1,-1)*size),smoothNormal(true)
{
#ifdef OpenCL
    this->kernel=NULL;
#endif

    buildFromFile(f);
}

PLYShape::PLYShape(double size,const Mark& mk)
    :Shape(mk),Lockable(),
     size(Treble<double>(1,1,1)*size),smoothNormal(true),box(NULL)
{
#ifdef OpenCL
    this->kernel=NULL;
#endif
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
#ifdef OpenCL
        if(boxes[i].pt!=NULL)
            free(boxes[i].pt);
#endif
        delete boxes[i].box;
    }

#ifdef OpenCL
    if(kernel!=NULL)
    {
        for(int i=0; i<Thread::nbThread(); i++)
            delete kernel[i];
        delete[] kernel;
    }
#endif
}

Hit PLYShape::_getHit(const Ray& r)const
{
    const PLYPrimitive* p=NULL;
    const PLYBox* b=NULL;
    Hit h=__getHit(r,&p,&b);

    if(!h.isNull()&&smoothNormal)
    {
        double dst=EPSILON;
        CollectionUnion<const PLYPrimitive*> prmUnion=CollectionUnion<const PLYPrimitive*>(2,&b->ht,&b->prm);
        double* cTab=(double*)malloc(prmUnion._count()*sizeof(double));
        for(int i=0; i<prmUnion._count(); i++)
        {
            bool flg=false;
            for(int j=0; !flg&&j<3; j++)
                for(int k=0; !flg&&k<3; k++)
                    if(Point(prmUnion[i]->pt[j])==Point(p->pt[k]))flg=true;
            if(flg)
            {
                const Point& d=prmUnion[i]->b;
                cTab[i]=h.getPoint().dist(d);
                dst=MAX(dst,p->b.dist(d));
            }
            else cTab[i]=-1;
        }
        Vector n=Vector::null;
        for(int i=0; i<prmUnion._count(); i++)
        {
            if(cTab[i]>0)
            {
                Vector w=prmUnion[i]->n;
                if(p->n.angle(w)>M_PI/4.0)w=-w;
                if(p->n.angle(w)<M_PI/4.0)
                    n+=w*SQ(1.0-cTab[i]/dst);
            }
        }
        free(cTab);

        h.setThNormal(n.isNull()?p->n:n.norm());
    }

    return h;
}

Hit PLYShape::__getHit(const Ray& r,const PLYPrimitive** p,const PLYBox** b)const
{
    Hit h=Hit::null;

    if(box==NULL||box->intersect(r))
    {
        double dMin=-1.0;
#ifdef OpenCL
        OpenCLKernel* krn=NULL;
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
                        for(int k=rand(); krn==NULL; k++)
                        {
                            if(this->kernel[k%Thread::nbThread()]->trylock()==0)
                            {
                                krn=this->kernel[k%Thread::nbThread()];

                                float k_r[2*TREBLE_SIZE];
                                for(int i=0; i<TREBLE_SIZE; i++)
                                    k_r[i]=(float)r.getPoint().get(i),k_r[TREBLE_SIZE+i]=(float)r.getVector().get(i);

                                krn->writeBuffer(0,2*TREBLE_SIZE,sizeof(float),k_r);
                            }
                        }

                        int cnt=largeBoxes[i]->boxes[j]->ht._count();
                        largeBoxes[i]->boxes[j]->lock.lock();
                        if(largeBoxes[i]->boxes[j]->pt==NULL)
                        {
                            largeBoxes[i]->boxes.getTab()[j]->pt=(float*)malloc(cnt*3*TREBLE_SIZE*sizeof(float));
                            for(int k=0; k<cnt; k++)
                                for(int l=0; l<3; l++)
                                    for(int m=0; m<TREBLE_SIZE; m++)
                                        largeBoxes[i]->boxes.getTab()[j]->pt[(((k*3)+l)*TREBLE_SIZE)+m]=(float)largeBoxes[i]->boxes[j]->ht[k]->pt[l].get(m);
                        }
                        largeBoxes[i]->boxes[j]->lock.unlock();

                        krn->writeBuffer(1,cnt*3*TREBLE_SIZE,sizeof(float),largeBoxes[i]->boxes[j]->pt);
                        krn->runKernel(cnt);
                        float *dst=(float*)malloc(cnt*sizeof(float));
                        krn->readBuffer(2,cnt,sizeof(float),dst);

                        for(int k=0; k<cnt; k++)
                        {
                            if(dst[k]>0&&((dst[k]<dMin)||(dMin<0)))
                            {
                                if(p!=NULL)*p=largeBoxes[i]->boxes[j]->ht[k];
                                if(b!=NULL)*b=largeBoxes[i]->boxes[j];
                                dMin=(double)dst[k];
                                h=Triangle::getTriangleHit(r,this,largeBoxes[i]->boxes[j]->ht[k]->pt);
                                h.setId(k);
                            }
                        }

                        free(dst);
#else
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
                                    dMin=d,h=ht;
                                }
                            }
                        }
#endif
                    }
                }
            }
        }
#ifdef OpenCL
        if(krn!=NULL)
        {
            krn->flush();
            krn->unlock();
        }
#endif
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
#ifdef OpenCL
    b.pt=NULL;
#endif
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

#ifdef OpenCL
    int nb=0;
    for(int i=0; i<this->largeBoxes._count(); i++)
        for(int j=0; j<this->largeBoxes[i]->boxes._count(); j++)
            nb=MAX(nb,this->largeBoxes[i]->boxes[j]->ht._count());

    this->kernel=new OpenCLKernel*[Thread::nbThread()];

    for(int i=0; i<Thread::nbThread(); i++)
    {
        this->kernel[i]=new OpenCLKernel("primitive_hit", "\
__kernel void primitive_hit(\
    __global const float *r,\
    __global const float *prm,\
    __global float *dst)\
{\
    int id=get_global_id(0);\
    dst[id]=-1.0;\
    float3 t_pt=vload3(0,r);\
    float3 t_vct=vload3(1,r);\
    float3 t_o=vload3(id*3,prm);\
    float3 t_v1=vload3((id*3)+1,prm)-t_o;\
    float3 t_v2=vload3((id*3)+2,prm)-t_o;\
    float d=dot(cross(t_v2,t_v1),t_vct);\
    float3 t_w=t_pt-t_o;\
    float a=dot(cross(t_v2,t_w),t_vct)/d;\
    if(a<0.0||a>1.0)return;\
    float b=dot(cross(t_w,t_v1),t_vct)/d;\
    if(b<0.0||b>1.0)return;\
    if((a+b)>1.0)return;\
    dst[id]=dot(cross(t_v1,t_v2),t_w)/d;\
}");

        this->kernel[i]->createBuffer(2*TREBLE_SIZE,sizeof(float),CL_MEM_READ_ONLY);
        this->kernel[i]->createBuffer(nb*3*TREBLE_SIZE,sizeof(float),CL_MEM_READ_ONLY);
        this->kernel[i]->createBuffer(nb,sizeof(float),CL_MEM_WRITE_ONLY);
    }
#endif
}
