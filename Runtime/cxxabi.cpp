//============
// cxxabi.cpp
//============

#include <cxxabi.h>


//=======
// Using
//=======

#include "Devices/System/System.h"
#include "MemoryHelper.h"
#include <assert.h>
#include <new>

using namespace Devices::System;


//===========
// Namespace
//===========

extern "C" {


//========
// Common
//========

VOID* __cxa_allocate_exception(SIZE_T thrown_size)noexcept
{
SIZE_T size=sizeof(UnwindException)+thrown_size;
auto exc=(UnwindException*)MemoryHelper::Allocate(size);
if(!exc)
	System::Restart();
return (BYTE*)exc+sizeof(UnwindException);
}

VOID __cxa_atexit() {}

VOID* __cxa_begin_catch(VOID* thrown)noexcept
{
return thrown;
}

VOID __cxa_call_terminate(VOID* exc)
{
System::Restart();
}

VOID __cxa_end_catch()noexcept {}

VOID __cxa_end_cleanup()noexcept {}

VOID* __cxa_get_exception_ptr(VOID* thrown)noexcept
{
auto exc=(UnwindException*)((SIZE_T)thrown-sizeof(UnwindException));
return exc->GetThrownException();
}

VOID __cxa_pure_virtual()
{
throw InvalidContextException();
}

VOID __cxa_throw(VOID* thrown, VOID* type, VOID (*destr)(VOID*))
{
auto exc=(UnwindException*)((SIZE_T)thrown-sizeof(UnwindException));
new (exc) UnwindException((TypeInfo const*)type, destr);
exc_save_context(&exc->Frame);
exc->Raise();
System::Restart();
}

VOID __cxa_throw_bad_array_new_length()
{
throw InvalidArgumentException();
}

}