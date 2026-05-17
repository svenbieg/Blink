//=============
// StatusLed.h
//=============

#pragma once


//=======
// Using
//=======

#include "Concurrency/Task.h"
#include "Devices/Timers/SystemTimer.h"
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
	using SystemTimer=Devices::Timers::SystemTimer;
	using Task=Concurrency::Task;
	using WifiAdapter=Devices::Wifi::WifiAdapter;

	// Con-/Destructors
	static inline Handle<StatusLed> Create() { return Global::Create(); }
	~StatusLed();

	// Common
	VOID Blink(UINT Period);
	inline BOOL IsBlinking()const { return m_Period>0; }
	inline BOOL IsOn()const { return m_On; }
	VOID Set(BOOL On);
	inline VOID Stop() { Blink(0); }

private:
	// Con-/Destructors
	friend Object;
	StatusLed();

	// Common
	VOID OnSystemTimer();
	VOID ServiceTask();
	Mutex m_Mutex;
	volatile BOOL m_On;
	UINT m_Period;
	Handle<Task> m_ServiceTask;
	Signal m_Signal;
	UINT m_Ticks;
	Handle<SystemTimer> m_Timer;
	Handle<WifiAdapter> m_WifiAdapter;
};

}}