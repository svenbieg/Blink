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

class System: public Object
{
public:
	// Common
	VOID Led(BOOL On);
	static Handle<System> Open();
	static VOID PowerOff();
	static VOID Reboot();
	static VOID Reset(ResetDevice Device);

private:
	// Con-/Destructors
	System();

	// Common
	static Handle<System> s_Default;
};

}}
