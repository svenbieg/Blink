//===============
// Application.h
//===============

#pragma once


//=======
// Using
//=======

#include "Devices/System/StatusLed.h"


//===========
// Namespace
//===========

namespace Blink {


//=============
// Application
//=============

class Application: public Object
{
public:
	// Using
	using StatusLed=Devices::System::StatusLed;

	// Con-/Destructors
	static inline Handle<Application> Create() { return new Application(); }

private:
	// Con-/Destructors
	Application();

	// Common
	Handle<StatusLed> m_StatusLed;
};

}