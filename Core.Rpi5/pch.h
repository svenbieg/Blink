//=======
// pch.h
//=======

#pragma once


//======================
// Forward-Declarations
//======================

typedef long int INT64;
typedef unsigned long int UINT64;

typedef unsigned int DWORD;
typedef unsigned long int QWORD;

typedef unsigned long int FILE_SIZE;
typedef unsigned long int SIZE_T;

typedef int OFFSET_T;

typedef double FLOAT_T;

#define offsetof(type, member) __builtin_offsetof(type, member)


//=======
// Using
//=======

#include "Core/Platform.h"


//========
// Common
//========

#define NO_DATA(align) ALIGN (align) SECTION(".nodata")

constexpr SIZE_T PAGE_SIZE=4096;
