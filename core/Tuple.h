#pragma once

#include "core.h"

#include <string.h>

#define TUPLE_IDX(_i,_S) for(tuple_idx<_S> _i;_i.isValid();++_i)

#define TUPLE_BINARY_OP(OP)						\
  Tuple operator OP(const Tuple& that)const				\
  {									\
    static struct tuple_binary_op:public binary_op<T,T>			\
    {									\
      virtual T operator()(const volatile T& t1,			\
			   const volatile T& t2)const			\
      {									\
	return t1 OP t2;						\
      }									\
    } _binary_op;							\
    return Tuple(*this,that,_binary_op);				\
  }

#define TUPLE_INPLACE_BINARY_OP(OP)					\
  Tuple& operator OP(const Tuple& that)					\
  {									\
    static struct tuple_inplace_binary_op:public inplace_binary_op<T>	\
    {									\
      virtual void operator()(T& t1,const T& t2)const			\
      {									\
	t1 OP t2;							\
      }									\
    } _inplace_binary_op;						\
    return tamper(that,_inplace_binary_op);				\
  }

#define TUPLE_FCT(FCT)							\
  static Tuple _##FCT(const Tuple& t1,const Tuple& t2)			\
  {									\
    static struct fct_binary_op:public binary_op<T,T>			\
    {									\
      virtual T operator()(const volatile T& t1,			\
			   const volatile T& t2)const			\
      {									\
	return FCT(t1,t2);						\
      }									\
    } _binary_op;							\
    return Tuple(t1,t2,_binary_op);					\
  }

template <typename T,typename U> struct unary_op
{
    virtual ~unary_op() {}
    virtual U operator()(const volatile T&)const=0;
};

template <typename T,typename U> struct binary_op
{
    virtual ~binary_op() {}
    virtual U operator()(const volatile T&,const volatile T&)const=0;
};

template <typename T> struct inplace_unary_op
{
    virtual ~inplace_unary_op() {}
    virtual void operator()(T&)const=0;
};

template <typename T> struct inplace_binary_op
{
    virtual ~inplace_binary_op() {}
    virtual void operator()(T&,const T&)const=0;
};

template <typename T> struct tuple_fct
{
    tuple_fct(const T& t=T()):f(t) {}
    virtual ~tuple_fct() {}
    operator T&()
    {
        return f;
    }
    T& operator*()
    {
        return f;
    }
private:
    T f;
};

template <typename T,typename U> struct unary_fct:public tuple_fct<U>
{
    virtual void operator()(const T&)=0;
};

template <typename T,typename U> struct binary_fct:public tuple_fct<U>
{
    virtual void operator()(const T&,const T&)=0;
};

template <int S> class tuple_idx
{
public:
    tuple_idx(int i=0)
    {
        reset(i);
    }
    void reset(int i=0)
    {
        this->i=i;
    }
    operator int()const
    {
        return i;
    }
    int operator*()const
    {
        return i;
    }
    tuple_idx& operator++()
    {
        ++i;
        return *this;
    }
    int operator+(int d)const
    {
        return (i+d)%S;
    }
    bool isValid()const
    {
        return i<S;
    }

protected:
    int i;
};

template <typename T,int S> struct unary_idx_op
{
    virtual ~unary_idx_op() {}
    virtual T operator()(const tuple_idx<S>&)const=0;
};

template <typename T,int S> struct tuple_idx_fct:public tuple_fct<T>
{
    virtual void operator()(const tuple_idx<S>&)=0;

    T& tamper()
    {
        TUPLE_IDX(i,S)(*this)(i);
        return (T&)(*this);
    }
};

template <typename T> struct TupleRef
{
    TupleRef(const T& t):t(t) {}
    operator const T&()const
    {
        return t;
    }
    const T& operator*()const
    {
        return t;
    }
    const T& t;
};

template <typename T> struct eq_binary_fct:public binary_fct<T,bool>
{
    eq_binary_fct()
    {
        (**this)=true;
    }
    virtual void operator()(const T& t1,const T& t2)
    {
        (**this)&=(t1==t2);
    }
};

template <> struct eq_binary_fct<double>:public binary_fct<double,bool>
{
    eq_binary_fct()
    {
        (**this)=true;
    }
    virtual void operator()(const double& t1,const double& t2)
    {
        (**this)&=FCMP(t1,t2);
    }
};

template <typename T,int TUPLE_SIZE> class Tuple
{
public:
    Tuple()
    {
        static struct tuple_inplace_unary_op:public inplace_unary_op<T>
        {
            virtual void operator()(T &t)const
            {
                t=T(0);
            }
        } _tuple_inplace_unary_op;
        Tuple::tamper(_tuple_inplace_unary_op);
    }
    explicit Tuple(T t)
    {
        struct tuple_inplace_unary_op:public inplace_unary_op<T>
        {
            tuple_inplace_unary_op(const T& t):_t(t) {}
            virtual void operator()(T &t)const
            {
                t=_t;
            }
            const T _t;
        } _tuple_inplace_unary_op(t);
        Tuple::tamper(_tuple_inplace_unary_op);
    }
    Tuple(const T *tab)
    {
        memcpy(this->vTab,tab,Tuple::PCK_SIZE);
    }
    Tuple(const struct inplace_unary_op<T>& op)
    {
        TUPLE_IDX(i,TUPLE_SIZE)op(this->vTab[i]);
    }
    template <typename U> Tuple(const volatile Tuple<U,TUPLE_SIZE>& that,const struct unary_op<U,T>& op)
    {
        TUPLE_IDX(i,TUPLE_SIZE)this->vTab[i]=op(that.get(i));
    }
    template <typename U> Tuple(const volatile Tuple<U,TUPLE_SIZE>& t1,const volatile Tuple<U,TUPLE_SIZE>& t2,
                                const struct binary_op<U,T>& op)
    {
        TUPLE_IDX(i,TUPLE_SIZE)this->vTab[i]=op(t1.get(i),t2.get(i));
    }
    Tuple(const struct unary_idx_op<T,TUPLE_SIZE>& op)
    {
        Tuple::tamper(op);
    }
    virtual ~Tuple() {}

public:
    bool operator==(const Tuple& that)const
    {
        eq_binary_fct<T> _binary_fct;
        return this->tamper<bool>(that,_binary_fct);
    }

    virtual bool isNull()const
    {
        return (*this)==Tuple::null;
    }
    T& operator[](int i)
    {
        return vTab[i];
    }
    const volatile T& get(int i)const volatile
    {
        return vTab[i];
    }
    const volatile T& operator()(int i)const volatile
    {
        return get(i);
    }

public:
    TUPLE_BINARY_OP(+);
    TUPLE_BINARY_OP(-);
    TUPLE_BINARY_OP(*);
    TUPLE_BINARY_OP(/);

    TUPLE_INPLACE_BINARY_OP(+=);
    TUPLE_INPLACE_BINARY_OP(-=);
    TUPLE_INPLACE_BINARY_OP(*=);
    TUPLE_INPLACE_BINARY_OP(/=);

    Tuple operator-()const volatile
    {
        static struct tuple_unary_op:public unary_op<T,T>
        {
            virtual T operator()(const volatile T& t)const
            {
                return -t;
            }
        } _unary_op;
        return Tuple(*this,_unary_op);
    }

    Tuple operator*(const T f)const volatile
    {
        struct tuple_unary_op:public unary_op<T,T>
        {
            tuple_unary_op(T f)
            {
                this->f=f;
            }
            virtual T operator()(const volatile T& t)const
            {
                return t*f;
            }
            T f;
        } _unary_op(f);
        return Tuple(*this,_unary_op);
    }

    Tuple& operator*=(const T f)
    {
        struct tuple_inplace_unary_op:public inplace_unary_op<T>
        {
            tuple_inplace_unary_op(T f)
            {
                this->f=f;
            }
            virtual void operator()(T& t)const
            {
                t*=f;
            }
            T f;
        } _inplace_unary_op(f);
        return tamper(_inplace_unary_op);
    }

    Tuple operator/(const T f)const volatile
    {
        return (*this)*(T(1)/f);
    }
    virtual Tuple& operator/=(const T f)
    {
        (*this)*=T(1)/f;
        return *this;
    }

public:
    T sum()const
    {
        struct tuple_unary_fct:public unary_fct<T,T>
        {
            virtual void operator()(const T& t)
            {
                (**this)+=t;
            }
        } _unary_fct;
        return this->tamper(_unary_fct);
    };

    T prod()const
    {
        struct tuple_unary_fct:public unary_fct<T,T>
        {
            tuple_unary_fct()
            {
                (**this)=T(1);
            }
            virtual void operator()(const T& t)
            {
                (**this)*=t;
            }
        } _unary_fct;
        return this->tamper(_unary_fct);
    };

    Tuple rot(int n=1)const
    {
        struct tuple_unary_idx_op:public unary_idx_op<T,TUPLE_SIZE>
        {
            tuple_unary_idx_op(const Tuple<T,TUPLE_SIZE> &t,int n):_t(t)
            {
                this->n=n;
            }
            virtual T operator()(const tuple_idx<TUPLE_SIZE> &i)const
            {
                return _t(i+n);
            }
            const Tuple<T,TUPLE_SIZE> &_t;
            int n;
        } _unary_idx_op(*this,n);
        return Tuple(_unary_idx_op);
    }

public:
    TUPLE_FCT(MIN);
    TUPLE_FCT(MAX);

public:
    T* getTab()
    {
        return vTab;
    }
    const T* _getTab()const
    {
        return vTab;
    }

public:
    Tuple& tamper(const Tuple& that,const struct inplace_binary_op<T>& op)
    {
        TUPLE_IDX(i,TUPLE_SIZE)op(this->vTab[i],that.vTab[i]);
        return *this;
    }

    Tuple& tamper(const struct inplace_unary_op<T>& op)
    {
        TUPLE_IDX(i,TUPLE_SIZE)op(this->vTab[i]);
        return *this;
    }

    template <typename U> Tuple<U,TUPLE_SIZE> tamper(const struct unary_op<U,T>& op)const
    {
        return Tuple<U,TUPLE_SIZE>(*this,op);
    }

    template <typename U> U tamper(struct unary_fct<T,U>& fct)const
    {
        TUPLE_IDX(i,TUPLE_SIZE)fct(this->vTab[i]);
        return fct;
    }

    template <typename U> U tamper(const Tuple<T,TUPLE_SIZE>& that,struct binary_fct<T,U>& fct)const
    {
        TUPLE_IDX(i,TUPLE_SIZE)fct(this->vTab[i],that.vTab[i]);
        return fct;
    }

    Tuple& tamper(const struct unary_idx_op<T,TUPLE_SIZE>& op)
    {
        TUPLE_IDX(i,TUPLE_SIZE)this->vTab[i]=op(i);
        return *this;
    }

protected:
    T vTab[TUPLE_SIZE];

public:
    static const Tuple null;
    static const Tuple Unit;
    static const int PCK_SIZE;
};

template <typename T,int S> inline Tuple<T,S> min(const Tuple<T,S>& t1,const Tuple<T,S>& t2)
{
    return Tuple<T,S>::_MIN(t1,t2);
}

template <typename T,int S> inline Tuple<T,S> max(const Tuple<T,S>& t1,const Tuple<T,S>& t2)
{
    return Tuple<T,S>::_MAX(t1,t2);
}

template <typename T,int S> inline T sum(const Tuple<T,S>& that)
{
    return that.sum();
}

template <typename T,int S> inline T prod(const Tuple<T,S>& that)
{
    return that.prod();
}

template <typename T,int S> inline Tuple<T,S> rot(const Tuple<T,S>& that,int n=1)
{
    return that.rot(n);
}

template <typename T> inline T get(const Tuple<T,1>& that)
{
    return that.get(0);
}

