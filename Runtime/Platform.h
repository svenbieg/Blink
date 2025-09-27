//============
// Platform.h
//============

#pragma once


//======================
// Forward-Declarations
//======================

#ifndef __always_inline
#define __always_inline __attribute__((always_inline)) inline
#endif

#ifndef __iram
#define __iram __attribute__((section(".iram")))
#endif

#ifndef __no_return
#define __no_return __attribute__((noreturn))
#endif

#define ALIGN(x) __attribute__((aligned(x)))
#define PACKED __attribute__((packed))
#define SECTION(x) __attribute__((section(x)))

#define NO_DATA(align) ALIGN (align) SECTION(".nodata")


//=======
// Using
//=======

#include "Default/Platform.h"
#include "TypeInfo.h"
