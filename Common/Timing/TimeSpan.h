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
	TimeSpan(UINT Seconds=0);
	TimeSpan(Handle<String> Name, UINT Seconds=0);

	// Access
	Handle<String> ToString()override;
	static Handle<String> ToString(UINT Seconds);
	static UINT ToString(UINT Seconds, LPSTR Buffer, UINT Size);
};

}