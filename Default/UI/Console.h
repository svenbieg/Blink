//===========
// Console.h
//===========

#pragma once


//=======
// Using
//=======

#include "Concurrency/Task.h"
#include "Devices/Serial/SerialPort.h"
#include "Global.h"


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

	// Friends
	friend class Global<Console>;

	// Common
	static inline Handle<Console> Get() { return s_Current; }
	static VOID Print(Handle<String> Text);
	template <class... _args_t> static inline VOID Print(LPCSTR Format, _args_t... Arguments)
		{
		auto str=String::Create(Format, Arguments...);
		Print(str);
		}

private:
	// Con-/Destructors
	Console();
	static Global<Console> s_Current;

	// Common
	Concurrency::Mutex m_Mutex;
	Handle<SerialPort> m_SerialPort;
};

}