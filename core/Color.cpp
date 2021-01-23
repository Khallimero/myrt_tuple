#include "Color.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

const Color Color::White=Color(1.0,1.0,1.0);
const Color Color::Black=Color(0.0,0.0,0.0);

Color::Color():Treble<double>(Color::Black) {}

Color::Color(const Tuple<double,TREBLE_SIZE>& that):Treble<double>(that) {}

Color::Color(const char* hex):Treble<double>()
{
    unsigned int v=0;
    sscanf(hex,"%x",&v);
    *this=Color(v);
}

Color::Color(const unsigned int v):Treble<double>()
{
    this->setRed(Color::denormVal((v&0xFF0000)>>16));
    this->setGreen(Color::denormVal((v&0xFF00)>>8));
    this->setBlue(Color::denormVal(v&0xFF));
}

Color::Color(double r,double g,double b):Treble<double>(r,g,b) {}

Color::Color(const double* tab):Treble<double>(tab) {}

Color::~Color() {}

Color::Color(FILE* f,const char* fmt,int norm_value)
{
    if(strcmp(fmt,"P3")==0)
    {
        struct color_inplace_unary_op:public inplace_unary_op<double>
        {
            color_inplace_unary_op(FILE *f,int v)
            {
                this->f=f,this->norm_value=v;
            }
            virtual void operator()(double &d)const
            {
                int c;
                fscanf(f,"%d",&c);
                d=Color::denormVal(c,norm_value);
            }
            FILE *f;
            int norm_value;
        };

        color_inplace_unary_op _inplace_unary_op(f,norm_value);
        *this=Color(_inplace_unary_op);
    }
    else if(strcmp(fmt,"P6")==0)
    {
        struct color_inplace_unary_op:public inplace_unary_op<double>
        {
            color_inplace_unary_op(FILE *f,int v)
            {
                this->f=f,this->norm_value=v;
            }
            virtual void operator()(double &d)const
            {
                unsigned char c;
                fscanf(f,"%c",&c);
                d=Color::denormVal(c,norm_value);
            }
            FILE *f;
            int norm_value;
        };

        color_inplace_unary_op _inplace_unary_op(f,norm_value);
        *this=Color(_inplace_unary_op);
    }
    else
    {
        fprintf(stderr,"Invalid format : %s\n",fmt);
        fflush(stderr);
        exit(1);
    }
}

void Color::write(FILE *f,const char* fmt,int norm_value)const
{
    bool flg=true;
    if(strcmp(fmt,"P3")==0)
    {
        struct color_tuple_idx_fct:public tuple_idx_fct<bool,TREBLE_SIZE>
        {
            color_tuple_idx_fct(const Color& c,FILE *f,int v):col(c)
            {
                this->f=f,this->norm_value=v;
                (**this)=true;
            }
            virtual void operator()(const tuple_idx<TREBLE_SIZE>& i)
            {
                if(i>0)(**this)&=(fprintf(f," "))>0;
                (**this)&=(fprintf(f,"%d",Color::normVal(col(i),norm_value))>0);
            }
            const Color& col;
            FILE *f;
            int norm_value;
        };
        color_tuple_idx_fct _tuple_idx_fct(*this,f,norm_value);
        flg&=_tuple_idx_fct.tamper();
        flg&=(fprintf(f,"\n")>0);
    }
    else if(strcmp(fmt,"P6")==0)
    {
        struct color_unary_fct:public unary_fct<double,bool>
        {
            color_unary_fct(FILE *f,int v)
            {
                this->f=f,this->norm_value=v;
                (**this)=true;
            }
            virtual void operator()(const double& c)
            {
                (**this)&=(fprintf(f,"%c",Color::normVal(c,norm_value))==1);
            }
            FILE *f;
            int norm_value;
        };
        color_unary_fct _unary_fct(f,norm_value);
        flg&=this->tamper<bool>(_unary_fct);
    }
    else
    {
        fprintf(stderr,"Invalid format : %s\n",fmt);
        fflush(stderr);
        exit(1);
    }
    if(!flg)
    {
        fprintf(stderr,"Error writing color to file\n");
        fflush(stderr);
        exit(1);
    }
}

Color Color::norm()const
{
    static struct norm:public unary_op<double,double>
    {
        virtual double operator()(const volatile double& c)const
        {
            return Color::_norm(c);
        }
    } color_norm;

    return Color(Tuple<double,TREBLE_SIZE>(*this,color_norm));
}

Color Color::negative()const
{
    return Color::White-(*this);
}

Color Color::beer(double bCoeff)const
{
    struct _beer:public unary_op<double,double>
    {
        _beer(double c)
        {
            this->bCoeff=c;
        }
        virtual double operator()(const volatile double& c)const
        {
            return exp((c-1.0)*bCoeff);
        }
        double bCoeff;
    };
    return Color(Tuple<double,TREBLE_SIZE>(*this,_beer(bCoeff)));
}

double Color::grey()const
{
    return this->sum()/(double)TREBLE_SIZE;
}
