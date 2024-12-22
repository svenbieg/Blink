//================
// MemoryHelper.h
//================

#pragma once


//========
// Macros
//========

#define ALIGN(x) __attribute__((aligned(x)))
#define PACKED __attribute__((packed))
#define SECTION(x) __attribute__((section(x)))

#define NO_DATA(align) ALIGN (align) SECTION(".nodata")
