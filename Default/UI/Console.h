//===========
// Console.h
//===========

#pragma once


//=======
// Using
//=======

#include "Concurrency/Task.h"
#include "Devices/Serial/SerialPort.h"


//===========
// Namespace
//===========

namespace UI {


//=========
// Console
//=========

class Console: public Object
{
public:
	// Using
	using SerialPort=Devices::Serial::SerialPort;

	// Common
	static Handle<Console> Get();
	static VOID Print(Handle<String> Text);
	template <class... _args_t> static inline VOID Print(LPCSTR Format, _args_t... Arguments)
		{
		auto str=String::Create(Format, Arguments...);
		Print(str);
		}

private:
	// Con-/Destructors
	Console();
	static Handle<Console> s_Current;
	static Concurrency::Mutex s_Mutex;

	// Common
	Concurrency::Mutex m_Mutex;
	Handle<SerialPort> m_SerialPort;
};

}