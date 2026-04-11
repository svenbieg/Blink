//=============
// StatusLed.h
//=============

#pragma once


//=======
// Using
//=======

#include "Global.h"


//===========
// Namespace
//===========

namespace Devices {
	namespace System {


//============
// Status-LED
//============

class StatusLed: public Global<StatusLed>
{
public:
	// Con-/Destructors
	static inline Handle<StatusLed> Create() { return Global::Create(); }

	// Common
	VOID Set(BOOL On);

private:
	// Con-/Destructors
	friend Object;
	StatusLed();
};

}}