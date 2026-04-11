//=============
// StatusLed.h
//=============

#pragma once


//=======
// Using
//=======

#include "Concurrency/Task.h"
#include "Devices/Wifi/WifiAdapter.h"


//===========
// Namespace
//===========

namespace Devices {
	namespace System {


//============
// Status-LED
//============

class StatusLed: public Global<StatusLed>
{
public:
	// Using
	using Mutex=Concurrency::Mutex;
	using Signal=Concurrency::Signal;
	using Task=Concurrency::Task;
	using WifiAdapter=Devices::Wifi::WifiAdapter;

	// Con-/Destructors
	static inline Handle<StatusLed> Create() { return Global::Create(); }
	~StatusLed();

	// Common
	inline BOOL IsOn()const { return m_On; }
	VOID Set(BOOL On);

private:
	// Con-/Destructors
	friend Object;
	StatusLed();

	// Common
	VOID ServiceTask();
	Mutex m_Mutex;
	volatile BOOL m_On;
	Handle<Task> m_ServiceTask;
	Signal m_Signal;
	Handle<WifiAdapter> m_WifiAdapter;
};

}}