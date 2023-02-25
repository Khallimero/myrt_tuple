#include "Renderer.h"
#include "RoundedIterator.h"
#include "AutoLock.h"

#include <stdio.h>
#include <string.h>

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

    Renderer::Monitored.add(this);
}

Renderer::~Renderer()
{
    Renderer::Monitored.replace(this,NULL);

    AutoLock autolock(this);
    if(pIt!=NULL)delete pIt;
}

SmartPointer<Picture> Renderer::render(const Scene* sc,const Camera& cam,
                                       const RendererQuality& quality)
{
    Renderer rnd(sc,cam,quality);
    rnd.pct=new Picture(cam.getVaX().getNb(),cam.getVaY().getNb());

    if(rnd.sc->getPhotonBoxIn()!=NULL)
    {
        rnd.setRenderingProgress(Photon);
        Thread::run(photonThread,&rnd);
        printf("Photon map : %d\n",rnd.photonMap.getNbPhotonHitCollection());
    }

    rnd.setRenderingProgress(Rendering);
    Thread::run(renderThread,&rnd);
    rnd.setRenderingProgress(Complete);

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
    ObjCollection<Ray> rc;
    rc._add(r);
    return computeRays(rc,nbRef)[0];
}

ObjCollection<Color> Renderer::computeRays(const ObjCollection<Ray>& r,int nbRef)const
{
    ObjCollection<Hit> hr=sc->getHit(r);
    ObjCollection<Color> cc=computeColors(hr,nbRef);
    if(nbRef<NB_REF)
    {
        for(int i=0; i<hr._count(); i++)
        {
            if(!hr[i].isNull())
            {
                double reflectCoeff=hr[i].getShape()->getReflectCoeff();

                if(hr[i].getShape()->getRefractCoeff()>0.0)
                {
                    Vector n=Vector(hr[i].getNormal()*SIGN(hr[i].getNormal().cosAngle(hr[i].getIncident().getVector()))).norm()*EPSILON;
                    double d1=sc->getDensity(hr[i].getPoint()-n);
                    double d2=sc->getDensity(hr[i].getPoint()+n);

                    Ray refract=hr[i].getRefract(d1,d2);
                    if(refract.isNull())
                    {
                        reflectCoeff+=hr[i].getShape()->getRefractCoeff();
                    }
                    else
                    {
                        Color cr=computeRay(refract,nbRef+1);
                        cc.getTab()[i]+=cr*hr[i].getShape()->getColor(hr[i])*hr[i].getShape()->getRefractCoeff();
                    }
                }

                if(reflectCoeff>0.0)
                {
                    Color cr=computeRay(hr[i].getReflect(),nbRef+1);
                    cc.getTab()[i]+=cr*reflectCoeff;
                }

                computeBeerColor(cc.getTab()[i],hr[i]);
            }
        }
    }

    return cc;
}

ObjCollection<Color> Renderer::computeColors(const ObjCollection<Hit>& hc,int nbRef)const
{
    ObjCollection<Color> cc(hc._count());

    for(int c=0; c<hc._count(); c++)
    {
        Color cl=Color::Black;
        const Hit& h=hc[c];
        if(!h.isNull())
        {
            Color ltSum=Color::Black,glSum=Color::Black,phSum=Color::Black;
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
                ObjCollection<Ray> rc,rrc;
                while(it->next())
                {
                    Vector w=Vector(l+(u*it->getElement(0))+(v*it->getElement(1))).norm();
                    if(sc->getLight(k)->getBox()==NULL||!sc->getLight(k)->getBox()->getHit(Ray(h.getPoint(),w)).isNull())
                    {
                        rc._add(Ray(p,w));
                        if(h.getShape()->getRefractCoeff()>0)
                            rrc._add(Ray(pr,w));
                    }
                }

                ObjCollection<Hit> hrc=sc->getIntersect(rc);
                for(int i=0; i<rc._count(); i++)
                {
                    double hDst=(hrc[i].isNull()?-1:p.dist(hrc[i].getPoint()));
                    if(hDst<0||(lDst>0&&hDst>0&&lDst<hDst))
                    {
                        double d=fabs(rc[i].getVector().cosAngle(h.getNormal()));
                        ltCol+=sc->getLight(k)->getColor()*mtg*d;

                        double a=rf.cosAngle(rc[i].getVector());
                        if(a>0)glCol+=sc->getLight(k)->getColor()*mtg*pow(a,h.getShape()->getGlareCoeff());
                    }
                }

                ObjCollection<Hit> hrrc=sc->getIntersect(rrc);
                for(int i=0; i<hrrc._count(); i++)
                {
                    double hDst=(hrrc[i].isNull()?-1:pr.dist(hrrc[i].getPoint()));
                    if(hDst<0||(lDst>0&&hDst>0&&lDst<hDst))
                    {
                        double d=fabs(rrc[i].getVector().cosAngle(h.getNormal()));
                        ltCol+=sc->getLight(k)->getColor()*mtg*d*h.getShape()->getRefractCoeff();
                    }
                }

                ltSum+=ltCol/(double)it->getActualSteps();
                glSum+=glCol/(double)it->getActualSteps();
            }

            ltSum+=sc->getAmbiant()*(0.5+fabs(h.getNormal().cosAngle(h.getThNormal()))/2.0)*(0.5+fabs(h.getNormal().cosAngle(h.getIncident().getVector()))/2.0);

            if(sc->getPhotonBoxIn()!=NULL)
            {
                NestedIterator<int,TREBLE_SIZE> pbIt;
                TUPLE_IDX(i,TREBLE_SIZE)
                pbIt.add(Iterator<int>(PhotonBox::getInd(h.getPoint().get(i)-PHOTON_RAD),
                                       PhotonBox::getInd(h.getPoint().get(i)+PHOTON_RAD)),i>=2);
                while(pbIt.next())
                {
                    ObjCollection<PhotonHit>* phc=photonMap.getPhotonHitCollection(h.getShape()->getId(),PhotonBox(pbIt.getTuple()));
                    if(phc!=NULL)
                        for(int ph=0; ph<phc->_count(); ph++)
                            if(h.getPoint().dist((*phc)[ph].getPoint())<PHOTON_RAD)
                            {
                                const Color& clp=(*phc)[ph].getColor();
                                const Vector& rp=(*phc)[ph].getVector();

                                double a=rp.reflect(h.getNormal()).cosAngle(-h.getIncident().getVector());
                                if(a>0)glSum+=clp*pow(a,h.getShape()->getGlareCoeff());

                                Color phCol=clp*fabs(rp.cosAngle(h.getNormal()));
                                if(rp.cosAngle(h.getIncident().getVector())>0)
                                    phSum+=phCol;
                                else if(h.getShape()->getRefractCoeff()>0)
                                    phSum+=phCol*h.getShape()->getRefractCoeff();
                            }
                }
                phSum*=h.getShape()->getDiffCoeff();
            }

            ltSum*=h.getShape()->getDiffCoeff();
            glSum*=h.getShape()->getSpecCoeff();
            cl+=(h.getShape()->getColor(h)*(ltSum+phSum))+glSum;
        }

        cc._add(cl);
    }

    return cc;
}

void Renderer::computeBeerColor(Color& c,const Hit& h)const
{
    Vector vct=h.getIncident().getVector().norm()*EPSILON;
    Point p1=h.getIncident().getPoint()+vct;
    Point p2=h.getPoint()-vct;
    double dst=p1.dist(p2);

    for(int i=0; i<sc->getNbShapes(); i++)
    {
        if(!sc->getShape(i)->isShell())
        {
            if(sc->getShape(i)->isInside(p1)&&sc->getShape(i)->isInside(p2))
            {
                double bc=1.0-(1.0/exp(pow(dst/sc->getShape(i)->getBeerSizeCoeff(),sc->getShape(i)->getBeerSizeExp())));
                c*=sc->getShape(i)->getBeerColor().beer(bc);
            }
        }
    }
}

void Renderer::computePhoton(const Hit& h,const Color& col,int nbRef)
{
    if(col.isNull())return;

    if(!h.isNull())
    {
        Color c=col;
        computeBeerColor(c,h);

        if(nbRef>0&&(sc->getPhotonBoxOut()==NULL||sc->getPhotonBoxOut()->isInside(h.getPoint())))
            photonMap.addPhotonHit(h.getShape()->getId(),PhotonHit(h.getPoint(),h.getIncident().getVector(),c));

        if(nbRef<PHOTON_REF)
        {
            double rCoeff=h.getShape()->getReflectCoeff();
            c*=h.getShape()->getColor(h);

            if(h.getShape()->getRefractCoeff()>0)
            {
                Vector n=Vector(h.getNormal()*SIGN(h.getNormal().cosAngle(h.getIncident().getVector()))).norm()*EPSILON;
                double d1=sc->getDensity(h.getPoint()-n);
                double d2=sc->getDensity(h.getPoint()+n);
                if(h.getRefract(d1,d2).isNull())rCoeff+=h.getShape()->getRefractCoeff();
                else computePhoton(sc->getHit(h.getRefract(d1,d2)),c*h.getShape()->getRefractCoeff(),nbRef+1);
            }

            c*=rCoeff+(fabs(h.getNormal().cosAngle(h.getIncident().getVector()))*RADIANCE_PHOTON_EXP);
            computePhoton(sc->getHit(h.getReflect()),c,nbRef+1);
        }
    }
}

void Renderer::computePhotons(const ObjCollection<Ray>& r,const Color& col,int nbRef)
{
    if(col.isNull())return;

    ObjCollection<Hit> h=sc->getHit(r);
    for(int i=0; i<h._count(); i++)
        computePhoton(h[i],col,nbRef);
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
            ObjCollection<Ray> rc;
            while(itLight->next())
                rc._add(rnd->sc->getLight(l)->getRay(rnd->sc->getPhotonBoxIn(),bpt,o,itLight));
            rnd->computePhotons(rc,rnd->sc->getLight(l)->getColor()*(double)PHOTON_EXP*SQ(rnd->sc->getPhotonBoxIn()->getRadius())
                                /(SQ(PHOTON_RAD)*MAX(NB_PHOTON_IT,1)*(rnd->sc->getLight(l)->getBox()!=NULL?NB_PHOTON_LIGHT:1)));
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
    ObjCollection<Ray> rc;
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
            rc._add(Ray(pf,pf.getVectorTo(f)));
        }
        fct+=it.getActualSteps();
    }

    ObjCollection<Color> cols=this->computeRays(rc);
    for(int i=0; i<cols._count(); i++)col+=cols[i];

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
