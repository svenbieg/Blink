//============
// Platform.h
//============

#pragma once


//======================
// Forward-Declarations
//======================

#ifndef __addr_t
#define __addr_t unsigned int
#endif

#define __force_inline __attribute__((always_inline)) inline

#ifndef __isense_t
#define __isense_t unsigned int
#endif

#define __ISR __attribute__((section(".data.isr")))

#ifndef __long
#define __long long
#endif

#ifndef __long_long
#define __long_long long long
#endif

#define COPY_MAX 2
