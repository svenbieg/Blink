//==========
// System.h
//==========

#pragma once


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
PcieHost=44
};


//========
// System
//========

class System
{
public:
	// Common
	static VOID Led(BOOL On);
	[[noreturn]] static VOID PowerOff();
	[[noreturn]] static VOID Reset();
	[[noreturn]] static VOID Restart();
	static VOID Reset(ResetDevice Device);
};

}}