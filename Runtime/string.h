//==========
// string.h
//==========

#pragma once



//=======
// Using
//=======

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif


//========
// Common
//========

int memcmp(void const* Buffer1, void const* Buffer2, size_t Size);
void* memcpy(void* Destination, void const* Source, size_t Size);
void* memmove(void* Destination, void const* Source, size_t Size);
void* memset(void* Destination, int Value, size_t Size);

#ifdef __cplusplus
}
#endif
