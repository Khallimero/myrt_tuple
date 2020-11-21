#include "Geometric.h"

Geometric::Geometric() {}

Geometric::Geometric(double x,double y,double z)
    :Arithmetic< Treble<double> >(Treble<double>(x,y,z)) {}

Geometric::Geometric(const Tuple<double,TREBLE_SIZE>& that)
    :Arithmetic< Treble<double> >(Treble<double>(that)) {}

Geometric::~Geometric() {}

double Geometric::prodScal(const Geometric& that)const
{
    struct treble_binary_fct:public binary_fct<double,double>
    {
        virtual void operator()(const double& t1,const double& t2)
        {
            (**this)+=t1*t2;
        }
    } _binary_fct;
    return this->tamper(that,_binary_fct);
}
