//==========
// stddef.h
//==========

#pragma once


//========
// Macros
//========

#ifndef __always_inline
#define __always_inline __attribute__((always_inline)) inline
#endif

#ifndef __long
#define __long
#endif

#ifndef __long_long
#define __long_long long
#endif

#define NULL 0

#define offsetof(type, member) __builtin_offsetof(type, member)


//=======
// Types
//=======

#ifndef __ASSEMBLER__

typedef unsigned __long_long int size_t;

#endif
