//=============
// StatusLed.h
//=============

#pragma once


//=======
// Using
//=======

#include "Concurrency/Task.h"
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
	using Task=Concurrency::Task;

	// Friends
	friend Object;

	// Con-/Destructors
	static inline Handle<StatusLed> Create() { return Global::Create(); }

	// Common
	VOID Blink(UINT Period);
	inline BOOL IsBlinking()const { return m_Period>0; }
	inline BOOL IsOn()const { return m_On; }
	VOID Set(BOOL On);
	VOID Stop();
	VOID Switch();

private:
	// Con-/Destructors
	StatusLed();

	// Common
	VOID BlinkTask();
	Handle<Task> m_BlinkTask;
	Mutex m_Mutex;
	BOOL m_On;
	UINT m_Period;
};

}}