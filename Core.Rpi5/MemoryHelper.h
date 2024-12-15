//================
// MemoryHelper.h
//================

#pragma once


//========
// Macros
//========

#define SECTION(x) __attribute__((section(x)))
#define NO_DATA(align) ALIGN (align) SECTION(".nodata")
