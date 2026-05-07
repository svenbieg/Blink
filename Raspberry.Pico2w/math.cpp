//==========
// math.cpp
//==========

#include <math.h>


//=======
// Using
//=======

#include "Exception.h"
#include "TypeHelper.h"


//========
// Common
//========

extern "C" double ceil(double x)
{
if(__builtin_isnan(x)||__builtin_isinf(x))
	throw InvalidArgumentException();
return -floor(-x);
}

extern "C" float ceilf(float x)
{
if(__builtin_isnan(x)||__builtin_isinf(x))
	throw InvalidArgumentException();
return -floorf(-x);
}

extern "C" double floor(double x)
{
if(__builtin_isnan(x)||__builtin_isinf(x))
	throw InvalidArgumentException();
union { double d; UINT64 u; } v;
v.d=x;
UINT64 ux=v.u;
WORD exp=(ux>>52)&0x7FF;
BYTE sign=ux>>63;
if(exp==0x7FF)
	throw InvalidArgumentException();
int e=(int)exp-1023;
if(e<0)
	return sign? -1.0: 0.0;
if(e>=52)
	return x;
int frac_bits=52-e;
UINT64 frac_mask=(1ULL<<frac_bits)-1;
UINT64 mant=ux&((1ULL<<52)-1);
if((mant&frac_mask)==0)
	return x;
UINT64 int_mant=mant&~frac_mask;
v.u=(ux&~((1ULL<<52)-1))|int_mant;
if(sign)
	return v.d-1.0;
return v.d;
}

extern "C" float floorf(float x)
{
if(__builtin_isnan(x)||__builtin_isinf(x))
	throw InvalidArgumentException();
union { float f; UINT u; } v;
v.f=x;
UINT ux=v.u;
WORD exp=(ux>>23)&0xFF;
BYTE sign=ux>>31;
if(exp==0xFF)
	throw InvalidArgumentException();
int e=(int)exp-127;
if(e<0)
	return sign? -1.0f: 0.0f;
if(e>=23)
	return x;
int frac_bits=23-e;
UINT frac_mask=(1U<<frac_bits)-1;
UINT mant=ux&((1U<<23)-1);
if((mant&frac_mask)==0)
	return x;
UINT int_mant=mant&~frac_mask;
v.u=(ux&~((1U<<23)-1))|int_mant;
if(sign)
	return v.f-1.f;
return v.f;
}

extern "C" long long llround(double x)
{
if(!__builtin_isfinite(x))
	throw InvalidArgumentException();
if(x>=0)
	{
	double y=floor(x+0.5);
	if(y>=(double)LLONG_MAX)
		throw OutOfRangeException();
	return (long long)y;
	}
else
	{
	double y=ceil(x-0.5);
	if(y<=(double)LLONG_MIN)
		throw OutOfRangeException();
	return (long long)y;
	}
}

extern "C" long long llroundf(float x)
{
if(!__builtin_isfinite(x))
	throw InvalidArgumentException();
if(x>=0)
	{
	float y=floorf(x+0.5);
	if(y>=(float)LLONG_MAX)
		throw OutOfRangeException();
	return (long long)y;
	}
else
	{
	float y=ceilf(x-0.5);
	if(y<=(float)LLONG_MIN)
		throw OutOfRangeException();
	return (long long)y;
	}
}
