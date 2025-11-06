//=======
// pch.h
//=======

#pragma once


//======================
// Forward-Declarations
//======================

#ifndef __LONG
#define __LONG
#endif

#ifndef __LONG_LONG
#define __LONG_LONG long
#endif

typedef unsigned __LONG_LONG int FILE_SIZE;
constexpr FILE_SIZE FILE_MAX=(FILE_SIZE)-1;

typedef unsigned __LONG_LONG int SIZE_T;
constexpr SIZE_T SIZE_MAX=(SIZE_T)-1;

struct COPY_T
{
COPY_T(SIZE_T Init)
	{
	Value[0]=Init;
	Value[1]=Init;
	}
SIZE_T Value[2];
};

constexpr unsigned int PAGE_SIZE=4096;


//=======
// Using
//=======

#include "Runtime/Platform.h"
