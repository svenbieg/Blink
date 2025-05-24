//=======
// pch.h
//=======

#pragma once


//======================
// Forward-Declarations
//======================

#ifndef __long
#define __long
#endif

#ifndef __long_long
#define __long_long long
#endif

typedef unsigned __long_long int FILE_SIZE;
constexpr FILE_SIZE FILE_MAX=0xFFFFFFFFFFFFFFFFULL;

constexpr unsigned int FLASH_ERASE=0xFFFFFFFFU;

typedef unsigned __long_long int SIZE_T;
constexpr SIZE_T SIZE_MAX=0xFFFFFFFFFFFFFFFFULL;

constexpr unsigned int PAGE_SIZE=4096;


//=======
// Using
//=======

#include "Runtime/Platform.h"
