#pragma once

#include "../myrt.h"

#include <math.h>
#include <stdlib.h>

#define MIN(a,b) ({typeof(a) _a=a;typeof(b) _b=b;_a>_b?_b:_a;})
#define MAX(a,b) ({typeof(a) _a=a;typeof(b) _b=b;_a<_b?_b:_a;})
#define SQ(x) ({typeof(x) _x=x;_x*_x;})
#define CB(x) ({typeof(x) _x=x;_x*_x*_x;})
#define SWAP(a,b) ({typeof(a) _a=a;typeof(b) _b=b;a=_b,b=_a;})
#define SIGN(x) (x<0?-1:1)
#define FCMP(a,b) (fabs((a)-(b))<=0.00000001)
#define FZERO(a) (FCMP(a,0))
#define RND ((double)rand()/RAND_MAX)
