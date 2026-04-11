//==========
// System.h
//==========

#pragma once


//=======
// Using
//=======

#include "TypeHelper.h"


//===========
// Namespace
//===========

namespace Devices {
	namespace System {


//=======
// Reset
//=======

enum class ResetDevice
{
None=0
};


//========
// System
//========

class System
{
public:
	// Common
	static VOID Enable(ResetDevice Device);
	[[noreturn]] static VOID PowerOff();
	[[noreturn]] static VOID Reset();
	[[noreturn]] static VOID Restart();
};

}}