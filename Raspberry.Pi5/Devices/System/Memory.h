//==========
// Memory.h
//==========

#pragma once


//=======
// Using
//=======

#include "Concurrency/CriticalMutex.h"


//======================
// Forward-Declarations
//======================

namespace Concurrency
{
class TaskMonitor;
}


//===========
// Namespace
//===========

namespace Devices {
	namespace System {


//======
// Info
//======

typedef struct
{
SIZE_T Available;
SIZE_T Total;
}MEMORY_INFO;


//========
// Memory
//========

class Memory
{
public:
	// Using
	using CriticalMutex=Concurrency::CriticalMutex;
	using TaskMonitor=Concurrency::TaskMonitor;

	// Friends
	friend TaskMonitor;

	// Common
	static VOID* Allocate(SIZE_T Size);
	static VOID Free(VOID* Buffer);
	static VOID GetInfo(MEMORY_INFO* Info);
	static VOID Initialize();
	static VOID* Uncached(VOID* Buffer);
	template <class _buf_t> inline static _buf_t* Uncached(_buf_t* Buffer) { return (_buf_t*)Uncached(Buffer); }

private:
	// Common
	static VOID* s_Heap;
	static CriticalMutex s_Mutex;
};

}}