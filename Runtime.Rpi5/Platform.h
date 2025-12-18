//============
// Platform.h
//============

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

#ifndef __SIZE_T
#define __SIZE_T unsigned long int
#endif

#ifndef __FILE_SIZE
#define __FILE_SIZE unsigned long int
#endif

#include "TypeHelper.h"
#include "MemoryHelper.h"

struct COPY_T
{
COPY_T(SIZE_T Init)
	{
	Value[0]=Init;
	Value[1]=Init;
	}
SIZE_T Value[2];
};

constexpr UINT PAGE_SIZE=4096;


//=======
// Using
//=======

#include <config.h>
#include "Default/Exception.h"
#include "Default/Event.h"
#include "Default/StatusHelper.h"
#include "Default/StringClass.h"
#include "Runtime/TypeInfo.h"
