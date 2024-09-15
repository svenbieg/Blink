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
	static VOID Enable();
	static VOID Initialize();

private:
	// Common
	static VOID CreatePageTable();
};

}}
