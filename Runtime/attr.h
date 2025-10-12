//========
// attr.h
//========

#pragma once


//============
// Attributes
//============

#define __align(x) __attribute__((aligned(x)))
#define __no_data(align) __attribute__((aligned(align))) __attribute__((section(".nodata")))
#define __section(x) __attribute__((section(x)))
