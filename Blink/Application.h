//===============
// Application.h
//===============

#pragma once


//=======
// Using
//=======

#include "Devices/System/StatusLed.h"
#include "UI/Console.h"


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
	using Console=UI::Console;
	using StatusLed=Devices::System::StatusLed;

	// Con-/Destructors
	static inline Handle<Application> Create() { return new Application(); }

	// Common
	VOID Blink();
	VOID Off();
	VOID On();
	VOID Run();
	VOID Stop();

private:
	// Con-/Destructors
	Application();

	// Common
	Handle<Console> m_Console;
	Handle<StatusLed> m_StatusLed;
};

}