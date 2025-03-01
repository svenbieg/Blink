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
void free(void* Buffer);
void* malloc(size_t Size);

#ifdef __cplusplus
}
#endif
