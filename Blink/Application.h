//===============
// Application.h
//===============

#pragma once


//=======
// Using
//=======

#include "Core/Application.h"


//===========
// Namespace
//===========

namespace Blink {


//=============
// Application
//=============

class Application: public Core::Application
{
public:
	// Con-/Destructors
	Application();

private:
	// Common
	VOID OnSystemTimerTick();
};

}
