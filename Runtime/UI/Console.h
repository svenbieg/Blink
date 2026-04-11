//===========
// Console.h
//===========

#pragma once


//=======
// Using
//=======

#include "Collections/map.hpp"
#include "Devices/Serial/SerialPort.h"
#include "Callable.h"
#include "Global.h"
#include "StringBuilder.h"


//===========
// Namespace
//===========

namespace UI {


//=========
// Console
//=========

class Console: public Global<Console>
{
public:
	// Using
	using ConsoleHandler=Callable<VOID>;
	using Mutex=Concurrency::Mutex;
	using SerialPort=Devices::Serial::SerialPort;

	// Con-/Destructors
	static inline Handle<Console> Get() { return Global::Create(); }

	// Common
	VOID AddCommand(Handle<String> Command, VOID (*Procedure)());
	template <class _owner_t> VOID AddCommand(Handle<String> Command, _owner_t* Owner, VOID (_owner_t::*Procedure)())
		{
		auto handler=CallableMemberFunction<_owner_t, VOID>::Create(Owner, Procedure);
		WriteLock lock(m_Mutex);
		m_Commands.add(Command, handler);
		}
	template <class _owner_t, class _lambda_t> VOID AddCommand(Handle<String> Command, _owner_t* Owner, _lambda_t&& Lambda)
		{
		auto handler=CallableLambda<_owner_t, _lambda_t, VOID>::Create(Owner, std::forward<_lambda_t>(Lambda));
		WriteLock lock(m_Mutex);
		m_Commands.add(Command, handler);
		}
	template <class _lambda_t> VOID AddCommand(Handle<String> Command, nullptr_t Owner, _lambda_t&& Lambda)
		{
		auto handler=CallableLambda<nullptr_t, _lambda_t, VOID>::Create(std::forward<_lambda_t>(Lambda));
		WriteLock lock(m_Mutex);
		m_Commands.add(Command, handler);
		}
	Event<Console, Handle<String>> CommandReceived;
	static VOID Print(LPCSTR Text);
	static VOID Print(Handle<String> Text);
	template <class... _args_t> static inline VOID Print(LPCSTR Format, _args_t... Arguments)
		{
		auto str=String::Create(Format, Arguments...);
		Print(str);
		}

private:
	// Con-/Destructors
	friend Object;
	Console();

	// Common
	VOID HandleCommand(Handle<String> Command);
	VOID OnSerialPortDataReceived();
	Collections::map<Handle<String>, Handle<ConsoleHandler>> m_Commands;
	Mutex m_Mutex;
	Handle<SerialPort> m_SerialPort;
	StringBuilder m_StringBuilder;
	Handle<Console> m_This;
};

}