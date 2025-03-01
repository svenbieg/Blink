//=======
// pch.h
//=======

#pragma once


//======================
// Forward-Declarations
//======================

#include <cstddef>

typedef std::size_t SIZE_T;
constexpr SIZE_T SIZE_MAX=0xFFFFFFFFFFFFFFFFULL;

typedef SIZE_T FILE_SIZE;

constexpr unsigned int PAGE_SIZE=4096;


//=======
// Using
//=======

#include "Runtime/Platform.h"
