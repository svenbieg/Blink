//========
// attr.h
//========

#pragma once


//============
// Attributes
//============

#define __align(x) __attribute__((aligned(x)))
#define __always_inline __attribute__((always_inline)) inline
#define __no_data(align) __attribute__((aligned(align))) __attribute__((section(".nodata")))
#define __no_return __attribute__((noreturn))
#define __packed __attribute__((packed))
#define __section(x) __attribute__((section(x)))
