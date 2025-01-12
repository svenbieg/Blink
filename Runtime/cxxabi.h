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

typedef UnwindStatus (*__gxx_personality_func_t)(INT Version, UnwindFlags Flags, UINT64 Class, UnwindException* Exception, UnwindContext* Context);

VOID* __cxa_allocate_exception(SIZE_T Size)noexcept;
VOID* __cxa_begin_catch(VOID* Thrown)noexcept;
VOID __cxa_atexit();
[[noreturn]] VOID __cxa_call_terminate(VOID* Exception);
VOID __cxa_end_catch()noexcept;
VOID* __cxa_get_exception_ptr(VOID* Thrown)noexcept;
[[noreturn]] VOID __cxa_throw(VOID* Thrown, VOID* Type, VOID (*Destructor)(VOID*));
UnwindStatus __gxx_personality_v0(INT Version, UnwindFlags Flags, UINT64 Class, UnwindException* Exception, UnwindContext* Context)noexcept;

}