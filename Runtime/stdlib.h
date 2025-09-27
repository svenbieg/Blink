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
extern "C" {
#endif

void abort();
void* aligned_alloc(size_t align, size_t size);
void free(void* buf);
void* malloc(size_t size);

#ifdef __cplusplus
}
#endif
