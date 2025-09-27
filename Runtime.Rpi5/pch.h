//=======
// pch.h
//=======

#pragma once


//======================
// Forward-Declarations
//======================

#include <stddef.h>

typedef unsigned __LONG_LONG int FILE_SIZE;
constexpr FILE_SIZE FILE_MAX=0xFFFFFFFFFFFFFFFFULL;

typedef unsigned __LONG_LONG int SIZE_T;
constexpr SIZE_T SIZE_MAX=0xFFFFFFFFFFFFFFFFULL;

constexpr unsigned int PAGE_SIZE=4096;


//=======
// Using
//=======

#include "Runtime/Platform.h"
