//==========
// Memory.h
//==========

#pragma once


//=======
// Using
//=======

#include "Peripherals.h"


//===========
// Namespace
//===========

namespace Devices {
	namespace System {


//========
// Memory
//========

class Memory
{
public:
	// Common
	static VOID Enable();
	static VOID Initialize();
	template <class _buf_t> inline static _buf_t* Uncached(_buf_t* Buffer) { return (_buf_t*)((SIZE_T)Buffer+UNCACHED_BASE); }

private:
	// Common
	static VOID CreatePageTable();
};

}}
