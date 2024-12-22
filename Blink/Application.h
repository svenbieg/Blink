//===============
// Application.h
//===============

#pragma once


//=======
// Using
//=======

#include "Firmware/Platform.h"


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
	Application();

private:
	// Common
	VOID OnSystemTimerTick();
};

}
