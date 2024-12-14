//==========
// stdlib.h
//==========

#pragma once


//=======
// Using
//=======

#include <stddef.h>


//========
// Common
//========

#ifdef __cplusplus
extern "C" [[noreturn]] void abort();
extern "C" void free(void* Buffer);
extern "C" void* malloc(size_t Size);
#else
[[noreturn]] extern void abort();
extern void free(void* Buffer);
extern void* malloc(size_t Size);
#endif
