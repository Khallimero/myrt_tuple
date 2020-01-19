#include "PLYShape.h"
#include "Triangle.h"
#include "Thread.h"
#include "AutoLock.h"

#include <math.h>
#include <stdio.h>

#define PLYBOX_RADIUS_FCT 10.0
#define LARGEBOX_RADIUS_FCT 10.0

PLYShape::PLYShape(const char* f,double size,const Mark& mk)
    :Shape(mk),Lockable(),
     size(Treble<double>(1,-1,-1)*size),smoothNormal(true)
{
    buildFromFile(f);
}

PLYShape::PLYShape(double size,const Mark& mk)
    :Shape(mk),Lockable(),
     size(Treble<double>(1,1,1)*size),smoothNormal(true),box(NULL)
{}

PLYShape::~PLYShape()
{
    for(int i=0; i<largeBoxes._count(); i++)
    {
        delete largeBoxes[i]->s;
        delete largeBoxes[i];
    }

    for(int i=0; i<boxes._count(); i++)
        delete boxes[i].s;
}

Hit PLYShape::_getHit(const Ray& r)const
{
    const PLYPrimitive* p=NULL;
    const PLYBox* b=NULL;
    Hit h=__getHit(r,&p,&b);

    if(!h.isNull()&&smoothNormal)
    {
        double dst=EPSILON;
        double* cTab=(double*)malloc(b->prm._count()*sizeof(double));
        for(int i=0; i<b->prm._count(); i++)
        {
            bool flg=false;
            for(int j=0; !flg&&j<3; j++)
                for(int k=0; !flg&&k<3; k++)
                    if(Point(b->prm[i]->pt[j])==Point(p->pt[k]))flg=true;
            if(flg)
            {
                const Point& d=b->prm[i]->b;
                cTab[i]=h.getPoint().dist(d);
                dst=MAX(dst,p->b.dist(d));
            }
            else cTab[i]=-1;
        }
        Vector n=Vector::null;
        for(int i=0; i<b->prm._count(); i++)
        {
            if(cTab[i]>0)
            {
                Vector w=b->prm[i]->n;
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
        for(int i=0; i<largeBoxes._count(); i++)
        {
            if(largeBoxes[i]->s->intersect(r))
            {
                for(int j=0; j<largeBoxes[i]->boxes._count(); j++)
                {
                    if(largeBoxes[i]->boxes[j]->s->intersect(r))
                    {
                        for(int k=0; k<largeBoxes[i]->boxes[j]->ht._count(); k++)
                        {
                            Hit ht=Triangle::getTriangleHit(r,this,largeBoxes[i]->boxes[j]->ht[k]->pt);
                            if(!(ht.isNull()))
                            {
                                if(ht.getId()<0)ht.setId(k);
                                double d=r.getPoint().dist(ht.getPoint());
                                if((h.isNull())||(d<dMin))
                                {
                                    if(p!=NULL)*p=largeBoxes[i]->boxes[j]->ht[k];
                                    if(b!=NULL)*b=largeBoxes[i]->boxes[j];
                                    if(largeBoxes[i]->boxes[j]->s->isInside(r.getPoint()))return ht;
                                    dMin=d,h=ht;
                                }
                            }
                        }
                    }
                }
            }
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
                if(!boxes[j].s->isInside(Point(shapes[i].pt[k])))flg=false;
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
            double d=largeBoxes[j]->s->getMark().getOrig().dist(boxes[i].s->getMark().getOrig())+boxes[i].s->getRadius();
            if(d<largeBoxes[j]->s->getRadius()&&(dMin<0||d<dMin))
            {
                dMin=d;
                lb=largeBoxes.getTab()[j];
            }
        }

        if(lb==NULL)
        {
            lb=new PLYLargeBox();
            lb->s=new Sphere(boxes[i].s->getRadius()*LARGEBOX_RADIUS_FCT,boxes[i].s->getMark());
            largeBoxes._add(lb);
        }
        lb->boxes._add(&(boxes[i]));
    }

    for(int i=0; i<largeBoxes._count(); i++)
    {
        double dMax=-1.0;
        for(int j=0; j<largeBoxes[i]->boxes._count(); j++)
            dMax=MAX(dMax,largeBoxes[i]->s->getMark().getOrig().dist(largeBoxes[i]->boxes[j]->s->getMark().getOrig())+largeBoxes[i]->boxes[j]->s->getRadius());
        largeBoxes.getTab()[i]->s->setRadius(dMax+EPSILON);
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
    b.s=new Sphere(r,Mark(pt));
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
                if(s->boxes[i].s->isInside(Point(s->shapes[k].pt[j])))n++;
            if(n>0)s->boxes.getTab()[i].prm._add(&s->shapes[k]);
            if(n==3)s->boxes.getTab()[i].ht._add(&s->shapes[k]);
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

