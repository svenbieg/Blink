//==========
// Memory.h
//==========

#pragma once


//=======
// Using
//=======

#include "Concurrency/ServiceMutex.h"


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
	using ServiceMutex=Concurrency::ServiceMutex;

	// Common
	static VOID* Allocate(SIZE_T Size);
	static VOID Free(VOID* Buffer);
	static VOID GetInfo(MEMORY_INFO* Info);
	static VOID Initialize();
	static VOID InitializeSecondary();
	static VOID* Uncached(VOID* Buffer);
	template <class _buf_t> inline static _buf_t* Uncached(_buf_t* Buffer) { return (_buf_t*)Uncached(Buffer); }

private:
	// Common
	static VOID CreatePageTable();
	static VOID EnableMmu();
	static VOID* s_Heap;
	static ServiceMutex s_Mutex;
};

}}