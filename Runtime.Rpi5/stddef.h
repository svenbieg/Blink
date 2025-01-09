//==========
// stddef.h
//==========

#pragma once


//========
// Macros
//========

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

typedef unsigned __long_long int size_t;
