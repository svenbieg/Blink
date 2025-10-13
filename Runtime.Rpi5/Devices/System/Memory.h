//==========
// Memory.h
//==========

#pragma once


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
	static VOID Initialize();
	static VOID* Uncached(VOID* Buffer);
	template <class _buf_t> inline static _buf_t* Uncached(_buf_t* Buffer) { return (_buf_t*)Uncached(Buffer); }
};

}}