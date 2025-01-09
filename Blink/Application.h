//===============
// Application.h
//===============

#pragma once


//=======
// Using
//=======

#include "Firmware/Application.h"


//===========
// Namespace
//===========

namespace Blink {


//=============
// Application
//=============

class Application: public Firmware::Application
{
public:
	// Con-/Destructors
	static inline Handle<Application> Create() { return new Application(); }

private:
	// Con-/Destructors
	Application();

	// Common
	VOID OnSystemTimerTick();
};

}