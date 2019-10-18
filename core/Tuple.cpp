#include "Tuple.h"

template <typename T,int S> const Tuple<T,S> Tuple<T,S>::null=Tuple<T,S>();
template <typename T,int S> const Tuple<T,S> Tuple<T,S>::Unit=Tuple<T,S>(T(1));
template <typename T,int S> const int Tuple<T,S>::PCK_SIZE=S*sizeof(T);

template class Tuple<double,UNIT_SIZE>;
template class Tuple<double,PAIR_SIZE>;
template class Tuple<double,TREBLE_SIZE>;
template class Tuple<int,UNIT_SIZE>;
template class Tuple<int,PAIR_SIZE>;
template class Tuple<int,TREBLE_SIZE>;
