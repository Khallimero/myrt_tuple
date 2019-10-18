#include "Geometric.h"

Geometric::Geometric() {}

Geometric::Geometric(double x,double y,double z)
    :Arithmetic< Treble<double> >(Treble<double>(x,y,z)) {}

Geometric::Geometric(const Tuple<double,TREBLE_SIZE>& that)
    :Arithmetic< Treble<double> >(Treble<double>(that)) {}

Geometric::~Geometric() {}
