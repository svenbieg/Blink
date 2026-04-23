//============
// Memory.cpp
//============

#include "Memory.h"


//=======
// Using
//=======

#include "Devices/Peripherals.h"
#include "Runtime/Configuration.h"
#include "MemoryHelper.h"
#include "heap.h"

extern COPY_T __flash_data_start;
extern COPY_T __data_start;
extern COPY_T __data_end;

extern COPY_T __bss_start;
extern COPY_T __bss_end;

extern SIZE_T __heap_start;

extern heap_t* g_heap;

typedef VOID (*CTOR_PTR)();

extern CTOR_PTR __init_array_start;
extern CTOR_PTR __init_array_end;


//===========
// Namespace
//===========

namespace Devices {
	namespace System {


//========
// Common
//========

VOID Memory::Initialize()
{
MemoryHelper::CopyT(&__data_start, &__flash_data_start, &__data_end);
MemoryHelper::ZeroT(&__bss_start, &__bss_end);
SIZE_T heap_start=(SIZE_T)&__heap_start;
SIZE_T heap_end=SRAM_END;
SIZE_T heap_size=heap_end-heap_start;
g_heap=heap_create(heap_start, heap_size);
for(CTOR_PTR* ctor=&__init_array_start; ctor!=&__init_array_end; ctor++)
	(*ctor)();
}

}}