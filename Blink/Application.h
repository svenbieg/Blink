//===============
// Application.h
//===============

#pragma once


//=======
// Using
//=======

#include "Concurrency/Task.h"


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
	using Task=Concurrency::Task;

	// Con-/Destructors
	static inline Handle<Application> Create() { return new Application(); }

	// Common
	VOID StartBlinking();
	VOID StopBlinking();

private:
	// Con-/Destructors
	Application();

	// Common
	Handle<Task> m_BlinkingTask;
};

}