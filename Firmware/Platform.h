//============
// Platform.h
//============

#pragma once


//=======
// Using
//=======

#include "Default/Platform.h"


//========
// Macros
//========

#define ALIGN(x) __attribute__((aligned(x)))
#define ALWAYS_INLINE __attribute__((always_inline)) inline
#define PACKED __attribute__((packed))
#define SECTION(x) __attribute__((section(x)))

#define NO_DATA(align) ALIGN (align) SECTION(".nodata")


//========
// Common
//========

#include "Concurrency/Task.h"
