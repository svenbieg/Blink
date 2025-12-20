//===========
// Console.h
//===========

#pragma once


//=======
// Using
//=======

#include "Collections/map.hpp"
#include "Devices/Serial/SerialPort.h"
#include "Function.h"
#include "Global.h"
#include "StringBuilder.h"


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
	VOID AddCommand(Handle<String> Command, Function<VOID()> Function);
	static inline Handle<Console> Get() { return s_Current; }
	Event<Console, Handle<String>> CommandReceived;
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
	VOID OnCommandReceived(Handle<String> Command);
	VOID OnSerialPortDataReceived();
	Collections::map<Handle<String>, Function<VOID()>> m_Commands;
	Concurrency::Mutex m_Mutex;
	Handle<SerialPort> m_SerialPort;
	StringBuilder m_StringBuilder;
};

}