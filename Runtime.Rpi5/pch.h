//=======
// pch.h
//=======

#pragma once


//======================
// Forward-Declarations
//======================

#include <stddef.h>

typedef size_t SIZE_T;
constexpr SIZE_T SIZE_MAX=0xFFFFFFFFFFFFFFFFULL;

typedef SIZE_T FILE_SIZE;

constexpr unsigned int PAGE_SIZE=4096;


//=======
// Using
//=======

#include "Firmware/Platform.h"
