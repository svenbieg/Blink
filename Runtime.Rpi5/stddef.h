//==========
// stddef.h
//==========

#pragma once


//========
// Macros
//========

#ifndef __LONG
#define __LONG
#endif

#ifndef __LONG_LONG
#define __LONG_LONG long
#endif

#define NULL 0

#define offsetof(type, member) __builtin_offsetof(type, member)


//=======
// Types
//=======

#ifndef __ASSEMBLER__

typedef __LONG_LONG int ptrdiff_t;
typedef unsigned __LONG_LONG int size_t;

#endif
