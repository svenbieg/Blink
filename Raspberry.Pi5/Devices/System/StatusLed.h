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
	inline VOID Stop() { Blink(0); }

private:
	// Con-/Destructors
	StatusLed();

	// Common
	VOID BlinkTask();
	BOOL m_On;
	UINT m_Period;
	Handle<Task> m_Task;
};

}}