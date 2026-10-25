//=========
// new.cpp
//=========

#include <new>


//=======
// Using
//=======

#include "Devices/System/Memory.h"
#include "Exception.h"

using namespace Devices::System;


//=====
// new
//=====

void* operator new(__isense_t size)
{
auto buf=Memory::Allocate(size);
if(!buf)
	throw OutOfMemoryException();
return buf;
}

void* operator new(__isense_t size, std::nothrow_t)noexcept
{
return Memory::Allocate(size);
}

void* operator new[](__isense_t size)
{
auto buf=Memory::Allocate(size);
if(!buf)
	throw OutOfMemoryException();
return buf;
}

void* operator new[](__isense_t size, std::nothrow_t)noexcept
{
return Memory::Allocate(size);
}

void operator delete(void* buf)noexcept
{
Memory::Free(buf);
}

void operator delete(void* buf, __isense_t)noexcept
{
Memory::Free(buf);
}

void operator delete[](void* buf)noexcept
{
Memory::Free(buf);
}

void operator delete[](void* buf, __isense_t)noexcept
{
Memory::Free(buf);
}
