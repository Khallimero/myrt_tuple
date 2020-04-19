#include "Renderer.h"
#include "RoundedIterator.h"
#include "AutoLock.h"

#include <stdio.h>
#include <string.h>

#ifndef CAUSTIC_PHOTON_EXP
#define CAUSTIC_PHOTON_EXP 1.0
#endif

#ifndef RADIANCE_PHOTON_EXP
#define RADIANCE_PHOTON_EXP 0.1
#endif

__thread NestedIterator<double,2>** Renderer::nestedItTab;
Collection <const Renderer*> Renderer::Monitored;

Renderer::Renderer(const Scene* sc,const Camera& cam,const RendererQuality& quality)
    :Lockable(),
     sc(sc),cam(cam),quality(quality)
{
    this->pIt=(sc->getPhotonBoxIn()==NULL?NULL:sc->getPhotonBoxIn()->getIterator(NB_PHOTON_IT));

    this->nIt.add(cam.getVaX().getIterator());
    this->nIt.add(cam.getVaY().getIterator());
    this->pct=NULL;

    this->setRenderingProgress(Init);
    this->rndProgressCount=0;
}

Renderer::~Renderer()
{
    AutoLock autolock(this);
    if(pIt!=NULL)delete pIt;
}

SmartPointer<Picture> Renderer::render(const Scene* sc,const Camera& cam,
                                       const RendererQuality& quality)
{
    Renderer rnd(sc,cam,quality);
    rnd.pct=new Picture(cam.getVaX().getNb(),cam.getVaY().getNb());
    Renderer::Monitored.add(&rnd);

    if(rnd.sc->getPhotonBoxIn()!=NULL)
    {
        rnd.setRenderingProgress(Photon);
        Thread::run(photonThread,&rnd);
        printf("Photon map : %d\n",rnd.photonMap.getNbPhotonHitCollection());
    }

    rnd.setRenderingProgress(Rendering);
    Thread::run(renderThread,&rnd);

    rnd.setRenderingProgress(Complete);
    Renderer::Monitored.replace(&rnd,NULL);

    return SmartPointer<Picture>(rnd.pct);
}

bool Renderer::getNextPhotonPoint(IteratorElement<double>* a,IteratorElement<double>* b)
{
    AutoLock autolock(this);
    bool hasNext=pIt->next();
    if(hasNext)
    {
        (*a)=pIt->getElement(0),(*b)=pIt->getElement(1);
        setProgressCount((a->getId())*(pIt->getIterator(1).getSteps())+(b->getId()));
    }
    return hasNext;
}

bool Renderer::getNextPoint(IteratorElement<double>* a,IteratorElement<double>* b)
{
    AutoLock autolock(this);
    bool hasNext=nIt.next();
    if(hasNext)
    {
        (*a)=nIt.getElement(0),
        (*b)=nIt.getElement(1);
        incRndProgressCount();
    }
    return hasNext;
}

Color Renderer::computeRay(const Ray& r,int nbRef)const
{
    Color col=Color::Black;

    Hit hr=sc->getHit(r);
    if(!hr.isNull())
    {
        col+=computeColor(hr,nbRef);

        if(nbRef<NB_REF)
        {
            double reflectCoeff=hr.getShape()->getReflectCoeff();

            if(hr.getShape()->getRefractCoeff()>0.0)
            {
                double d1=sc->getDensity(hr.getPoint()-(r.getVector().norm()*EPSILON));
                double d2=sc->getDensity(hr.getPoint()+(r.getVector().norm()*EPSILON));

                Ray refract=hr.getRefract(d1,d2);
                if(refract.isNull())
                {
                    reflectCoeff+=hr.getShape()->getRefractCoeff();
                }
                else
                {
                    Color cr=computeRay(refract,nbRef+1);
                    col+=cr*hr.getShape()->getColor(hr)*hr.getShape()->getRefractCoeff();
                }
            }

            if(reflectCoeff>0.0)
            {
                Color cr=computeRay(hr.getReflect(),nbRef+1);
                col+=cr*reflectCoeff;
            }
        }

        col=computeBeerCoeff(col,hr);
    }

    return col;
}

Color Renderer::computeColor(const Hit& h,int nbRef)const
{
    Color ltSum=Color::Black,glSum=Color::Black,dlSum=Color::Black,phSum=Color::Black;
    Vector rf=h.getIncident().getVector().reflect(h.getNormal());

    Vector n=Vector(h.getNormal()*SIGN(h.getNormal().cosAngle(h.getIncident().getVector()))).norm()*EPSILON;
    Point p=h.getPoint()-n;
    Point pr=h.getPoint()+n;

    for(int k=0; k<sc->getNbLights(); k++)
    {
        Color ltCol=Color::Black,glCol=Color::Black;

        double lDst=sc->getLight(k)->dist(p);
        double mtg=sc->getLight(k)->getMitigation(lDst);
        Vector l=sc->getLight(k)->getVectorTo(p);
        Vector u=l.getOrtho();
        Vector v=l.prodVect(u).norm();

        NestedIterator<double,2> *it=(nbRef<MAX_REF_SS?nestedItTab[k]:nestedItTab[sc->getNbLights()]);
        it->reset();
        while(it->next())
        {
            Vector w=Vector(l+(u*it->getElement(0))+(v*it->getElement(1))).norm();
            if(sc->getLight(k)->getBox()!=NULL)
            {
                Hit hl=sc->getLight(k)->getBox()->getHit(Ray(h.getPoint(),w));
                if(hl.isNull())continue;
                lDst=p.dist(hl.getPoint());
            }

            Hit hr=sc->getHit(Ray(p,w));
            double hDst=(hr.isNull()?-1:p.dist(hr.getPoint()));
            if(hDst<0||(lDst>0&&hDst>0&&lDst<hDst))
            {
                double d=MAX(w.cosAngle(h.getNormal()),w.cosAngle(-h.getNormal()));
                ltCol+=sc->getLight(k)->getColor()*mtg*d;

                double a=rf.cosAngle(w);
                if(a>0)glCol+=sc->getLight(k)->getColor()*mtg*pow(a,h.getShape()->getGlareCoeff());
            }

            if(h.getShape()->getRefractCoeff()>0&&
                    SIGN(h.getNormal().cosAngle(h.getIncident().getVector()))==SIGN(h.getNormal().cosAngle(w)))
            {
                Hit hr=sc->getHit(Ray(pr,w));
                double hDst=(hr.isNull()?-1:pr.dist(hr.getPoint()));
                if(hDst<0||(lDst>0&&hDst>0&&lDst<hDst))
                {
                    double d=MAX(w.cosAngle(h.getNormal()),w.cosAngle(-h.getNormal()));
                    ltCol+=sc->getLight(k)->getColor()*mtg*d*h.getShape()->getRefractCoeff();
                }
            }
        }

        ltSum+=ltCol/(double)it->getActualSteps();
        glSum+=glCol/(double)it->getActualSteps();
    }

    ltSum+=sc->getAmbiant()*(.5+fabs(h.getNormal().cosAngle(h.getThNormal()))/2.)*(.5+fabs(h.getNormal().cosAngle(h.getIncident().getVector()))/2.);

    if(sc->getPhotonBoxIn()!=NULL)
    {
        int s1=SIGN(h.getNormal().cosAngle(-h.getIncident().getVector()));
        NestedIterator<int,TREBLE_SIZE> pbIt;
        TUPLE_IDX(i,TREBLE_SIZE)
        pbIt.add(Iterator<int>(PhotonBox::getInd(h.getPoint().get(i)-PHOTON_RAD),
                               PhotonBox::getInd(h.getPoint().get(i)+PHOTON_RAD)),i>=2);
        while(pbIt.next())
        {
            const ObjCollection<PhotonHit>* phc=photonMap.getPhotonHitCollection(h.getShape()->getId(),PhotonBox(pbIt.getTuple()));
            if(phc!=NULL)
            {
                for(int ph=0; ph<phc->_count(); ph++)
                {
                    if(h.getPoint().dist((*phc)[ph].getPoint())<PHOTON_RAD)
                    {
                        Color cl=(*phc)[ph].getColor();
                        Vector rp=(*phc)[ph].getVector();
                        double a=rf.cosAngle(rp);
                        if(a>0)glSum+=cl*pow(a,h.getShape()->getGlareCoeff());

                        a=h.getNormal().cosAngle(rp);
                        int s2=SIGN(a);
                        if(s1!=s2)cl*=h.getShape()->getRefractCoeff();
                        if(!cl.isNull())phSum+=cl*fabs(a);
                    }
                }
            }
        }
        phSum*=h.getShape()->getDiffCoeff();
    }

    ltSum*=h.getShape()->getDiffCoeff();
    glSum*=h.getShape()->getSpecCoeff();
    Color cl=(h.getShape()->getColor(h)*(ltSum+phSum))+glSum+dlSum;

    return cl;
}

Color Renderer::computeBeerCoeff(const Color& c,const Hit& h)const
{
    Color col=c;
    const Point& p1=h.getIncident().getPoint();
    Point p2=h.getPoint()-(h.getIncident().getVector().norm()*EPSILON);
    double dst=p1.dist(p2);

    for(int i=0; i<sc->getNbShapes(); i++)
    {
        if(!sc->getShape(i)->isShell())
        {
            if(sc->getShape(i)->isInside(p1)&&sc->getShape(i)->isInside(p2))
            {
                double bc=pow(dst/sc->getShape(i)->getBeerSizeCoeff(),1.0/sc->getShape(i)->getBeerAbsorbCoeff());
                col*=sc->getShape(i)->getBeerColor().beer(bc);
            }
        }
    }
    return col;
}

void Renderer::computePhoton(const Ray& r,const Color& col,int nbRef)
{
    if(col.isNull())return;

    Hit h=sc->getHit(r);
    if(!h.isNull())
    {
        Color c=computeBeerCoeff(col,h);

        if(nbRef>0&&(sc->getPhotonBoxOut()==NULL||sc->getPhotonBoxOut()->isInside(h.getPoint())))
            photonMap.addPhotonHit(h.getShape()->getId(),PhotonHit(h.getPoint(),-r.getVector(),c));

        if(nbRef<PHOTON_REF)
        {
            double rCoeff=h.getShape()->getReflectCoeff();
            c*=h.getShape()->getColor(h);

            if(h.getShape()->getRefractCoeff()>0)
            {
                double d1=sc->getDensity(h.getPoint()-(h.getIncident().getVector().norm()*EPSILON));
                double d2=sc->getDensity(h.getPoint()+(h.getIncident().getVector().norm()*EPSILON));
                if(h.getRefract(d1,d2).isNull())rCoeff+=h.getShape()->getRefractCoeff();
                else computePhoton(h.getRefract(d1,d2),
                                       c*h.getShape()->getRefractCoeff()*CAUSTIC_PHOTON_EXP,
                                       nbRef+1);
            }

            double a=fabs(h.getNormal().cosAngle(r.getVector()));
            rCoeff+=a*RADIANCE_PHOTON_EXP;//radiance

            if(rCoeff>0)computePhoton(h.getReflect(),c*rCoeff,nbRef+1);
        }
    }
}

void* photonThread(void* d)
{
    Renderer* rnd=(Renderer*)d;

    rnd->nestedItTab=NULL;
    pthread_cleanup_push(renderThreadCleanup,rnd);
    rnd->setLightIterator(NB_PHOTON_LIGHT);

    Point o=rnd->sc->getPhotonBoxIn()->getMark().getOrig();
    IteratorElement<double> i,j;
    while(rnd->getNextPhotonPoint(&i,&j))
    {
        for(int l=0; l<rnd->sc->getNbLights(); l++)
        {
            Point bpt=rnd->sc->getPhotonBoxIn()->getPoint(rnd->sc->getLight(l)->getOrig(rnd->sc->getPhotonBoxIn(),o),i,j);
            NestedIterator<double,2> *itLight=rnd->nestedItTab[l];
            itLight->reset();
            while(itLight->next())
            {
                Ray r=rnd->sc->getLight(l)->getRay(rnd->sc->getPhotonBoxIn(),bpt,o,itLight);
                rnd->computePhoton(r,rnd->sc->getLight(l)->getColor()*(double)PHOTON_EXP*SQ(rnd->sc->getPhotonBoxIn()->getRadius())
                                   /(SQ(PHOTON_RAD)*MAX(NB_PHOTON_IT,1)*(rnd->sc->getLight(l)->getBox()!=NULL?NB_PHOTON_LIGHT:1)));
            }
        }
    }

    pthread_cleanup_pop(1);

    return NULL;
}

Color Renderer::renderPoint(const Vector& vct)const
{
    int fct=0;
    Color col=Color::Black;
    Point p0=this->cam.getOrig()+(this->cam.getDir()+vct);
    NestedIterator<double,2> aa(Iterator<double>(this->quality.getAliasing(),1.0/(double)this->quality.getAliasing()),2);
    while(aa.next())
    {
        Point pa=p0+(this->cam.getVaX().getElem()*aa[0])+(this->cam.getVaY().getElem()*aa[1]);
        Point f(this->cam.getOrig()+this->cam.getOrig().getVectorTo(pa).norm()*this->cam.getFocusDist());

        RoundedIterator it=(this->cam.getLenSize()<0?
                            RoundedIterator(Iterator<double>(1,0)):
                            RoundedIterator(Iterator<double>(this->quality.getFocus(),this->cam.getLenSize()/(double)this->quality.getFocus())));
        while(it.next())
        {
            Point pf=pa+(this->cam.getVaX().getElem()*it[0])+(this->cam.getVaY().getElem()*it[1]);
            Ray r=Ray(pf,pf.getVectorTo(f));
            col+=this->computeRay(r);
        }
        fct+=it.getActualSteps();
    }

    return col/(double)fct;
}

void Renderer::setRenderingProgress(RenderingProgress rp)
{
    AutoLock autolock(this);
    this->rndProgress=rp;
    this->progressCount=0;
    this->startTime=time(NULL);
}

void Renderer::incProgressCount()
{
    this->progressCount++;
}

void Renderer::setProgressCount(int p)
{
    this->progressCount=p;
}

void Renderer::incRndProgressCount()
{
    this->rndProgressCount++;
}

void Renderer::setLightIterator(int n)
{
    nestedItTab=(NestedIterator<double,2>**)malloc((sc->getNbLights()+1)*sizeof(NestedIterator<double,2>*));
    for(int i=0; i<sc->getNbLights(); i++)
        nestedItTab[i]=sc->getLight(i)->getIterator(n);
    nestedItTab[sc->getNbLights()]=new NestedIterator<double,2>(Iterator<double>(1,0),2);
}

void* renderThread(void* d)
{
    Renderer* rnd=(Renderer*)d;

    rnd->nestedItTab=NULL;
    pthread_cleanup_push(renderThreadCleanup,rnd);
    rnd->setLightIterator(SQ(rnd->quality.getShadow()));

    IteratorElement<double> i,j;
    while(rnd->getNextPoint(&i,&j))
    {
        Vector vct=rnd->cam.getVector(i,j);
        (*(rnd->pct))[i.getId()][j.getId()]=rnd->renderPoint(vct);
    }

    pthread_cleanup_pop(1);

    return NULL;
}

void renderThreadCleanup(void* d)
{
    Renderer* rnd=(Renderer*)d;

    for(int i=0; i<rnd->sc->getNbLights()+1; i++)
        delete rnd->nestedItTab[i];
    free(rnd->nestedItTab);
    rnd->nestedItTab=NULL;
}
