//========
// math.c
//========

#include <math.h>


//=======
// Using
//=======

#include <limits.h>
#include <float.h>
#include <stdint.h>


//========
// Common
//========

double ceil(double x)
{
if(isnan(x)||isinf(x))
	return x;
return -floor(-x);
}

double floor(double x)
{
union { double d; uint64_t u; } v;
v.d=x;
uint64_t ux=v.u;
uint16_t exp=(ux>>52)&0x7FF;
uint8_t sign=ux>>63;
if(exp==0x7FF)
	return x;
int e=(int)exp-1023;
if(e<0)
	return sign? -1.0: 0.0;
if(e>=52)
	return x;
int frac_bits=52-e;
uint64_t frac_mask=((1ULL<<frac_bits)-1);
uint64_t mant=ux&((1ULL<<52)-1);
if((mant&frac_mask)==0)
	return x;
uint64_t int_mant=mant&~frac_mask;
v.u=(ux&~((1ULL<<52)-1))|int_mant;
if(sign)
	return -v.d;
return v.d;
}

long long llround(double x)
{
if(!isfinite(x))
	{
	if(isnan(x))
		return 0;
	return x>0? LLONG_MAX: LLONG_MIN;
	}
if(x>=0)
	{
	double y=floor(x+0.5);
	if(y>=(double)LLONG_MAX)
		return LLONG_MAX;
	return (long long)y;
	}
else
	{
	double y=ceil(x-0.5);
	if(y<=(double)LLONG_MIN)
		return LLONG_MIN;
	return (long long)y;
	}
}
