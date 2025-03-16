//============
// Platform.h
//============

#pragma once


//======================
// Forward-Declarations
//======================

#define ALIGN(x) __attribute__((aligned(x)))
#define ALWAYS_INLINE __attribute__((always_inline)) inline
#define PACKED __attribute__((packed))
#define SECTION(x) __attribute__((section(x)))

#define IRAM SECTION(".iram")
#define NO_DATA(align) ALIGN (align) SECTION(".nodata")


//=======
// Using
//=======

#include "Default/Platform.h"
#include "TypeInfo.h"
