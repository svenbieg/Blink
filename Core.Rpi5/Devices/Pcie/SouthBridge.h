//===============
// SouthBridge.h
//===============

#pragma once


//===========
// Namespace
//===========

namespace Devices {
	namespace Pcie {


//==============
// South-Bridge
//==============

class SouthBridge: public Object
{
public:
	// Common
	static Handle<SouthBridge> Open();

private:
	// Con-/Destructors
	SouthBridge();

	// Common
	static Handle<SouthBridge> s_Default;
};

}}
