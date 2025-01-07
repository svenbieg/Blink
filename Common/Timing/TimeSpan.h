//============
// TimeSpan.h
//============

#pragma once


//=======
// Using
//=======

#include "Variable.h"


//===========
// Namespace
//===========

namespace Timing {


//===========
// Time-Span
//===========

class TimeSpan: public TypedVariable<UINT>
{
public:
	// Con-/Destructors
	static inline Handle<TimeSpan> Create(UINT Seconds=0) { return new TimeSpan(nullptr, Seconds); }
	static inline Handle<TimeSpan> Create(Handle<String> Name, UINT Seconds=0) { return new TimeSpan(Name, Seconds); }

	// Access
	Handle<String> ToString()override;
	static Handle<String> ToString(UINT Seconds);
	static UINT ToString(UINT Seconds, LPSTR Buffer, UINT Size);

private:
	// Con-/Destructors
	TimeSpan(Handle<String> Name, UINT Seconds): TypedVariable(Name, Seconds) {}
};

}