//=============
// StatusLed.h
//=============

#pragma once


//=======
// Using
//=======

#include "Concurrency/Signal.h"
#include "Concurrency/Task.h"
#include "Devices/Wifi/WifiAdapter.h"
#include "Global.h"


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
	VOID Blink(UINT Period);
	inline BOOL IsBlinking()const { return m_Period>0; }
	inline BOOL IsOn()const { return m_On; }
	VOID Set(BOOL On);
	VOID Stop();
	VOID Switch();

private:
	// Con-/Destructors
	friend Object;
	StatusLed();

	// Common
	VOID BlinkTask();
	VOID ServiceTask();
	Handle<Task> m_BlinkTask;
	Mutex m_Mutex;
	BOOL m_On;
	UINT m_Period;
	Handle<Task> m_ServiceTask;
	Signal m_Signal;
	Handle<WifiAdapter> m_Wifi;
};

}}