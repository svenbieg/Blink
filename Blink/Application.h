//===============
// Application.h
//===============

#pragma once


//=======
// Using
//=======

#include "Collections/map.hpp"
#include "Concurrency/Task.h"
#include "Function.h"
#include "Global.h"


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

	// Friends
	friend class Global<Application>;

	// Common
	static inline Handle<Application> Get() { return s_Current; }
	VOID StartBlinking();
	VOID StopBlinking();
	VOID Run();

private:
	// Con-/Destructors
	Application();
	static Global<Application> s_Current;

	// Common
	VOID InitializeCommands();
	VOID OnConsoleCommandReceived(Handle<String> Command);
	Handle<Task> m_BlinkingTask;
	Collections::map<Handle<String>, Function<VOID()>> m_Commands;
};

}