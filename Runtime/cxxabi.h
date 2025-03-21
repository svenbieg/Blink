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

extern "C" {


//========
// Common
//========

VOID* __cxa_allocate_exception(SIZE_T Size)noexcept;
VOID* __cxa_begin_catch(VOID* Thrown)noexcept;
VOID __cxa_atexit();
[[noreturn]] VOID __cxa_call_terminate(VOID* Exception);
VOID __cxa_end_catch()noexcept;
VOID __cxa_end_cleanup()noexcept;
VOID* __cxa_get_exception_ptr(VOID* Thrown)noexcept;
VOID __cxa_pure_virtual();
[[noreturn]] VOID __cxa_throw(VOID* Thrown, VOID* Type, VOID (*Destructor)(VOID*));
VOID __gxx_personality_v0(INT Version, UINT Flags, UINT64 Class, UnwindException* Exception, UnwindContext* Context)noexcept;

}