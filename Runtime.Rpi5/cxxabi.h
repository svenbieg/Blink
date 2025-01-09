//==========
// cxxabi.h
//==========

#pragma once


//=======
// Using
//=======

#include <unwind.h>


//===========
// Namespace
//===========

namespace Runtime {


//========
// Common
//========

extern "C"
{
VOID* __cxa_allocate_exception(SIZE_T Size)noexcept;
VOID* __cxa_begin_catch(VOID* Thrown);
VOID __cxa_atexit();
VOID __cxa_end_catch();
VOID __cxa_free_exception(VOID* Thrown)noexcept;
VOID __cxa_throw(VOID* Thrown, VOID* Type, VOID (*Destructor)(VOID*));
UnwindStatus __gxx_personality_v0(INT Version, UnwindFlags Flags, UINT64 Class, UnwindException* Exception, UnwindContext* Context)noexcept;
}

}