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

class TimeSpan: public Variable
{
public:
	// Con-/Destructors
	static inline Handle<TimeSpan> Create(UINT Seconds=0) { return new TimeSpan(nullptr, Seconds); }
	static inline Handle<TimeSpan> Create(Handle<String> Name, UINT Seconds=0) { return new TimeSpan(Name, Seconds); }

	// Access
	UINT Get();
	static inline UINT Get(TimeSpan* Value) { return Value? Value->Get(): 0; }
	inline Handle<String> GetName()const override { return m_Name; }
	Event<TimeSpan, UINT&> Reading;
	inline Handle<String> ToString(LanguageCode Language=LanguageCode::None)override { return ToString(Get()); }
	static Handle<String> ToString(UINT Seconds);
	static UINT ToString(LPSTR Buffer, UINT Size, UINT Seconds);
	SIZE_T WriteToStream(OutputStream* Stream)override;

	// Modification
	SIZE_T ReadFromStream(InputStream* Stream, BOOL Notify=true)override;
	BOOL Set(UINT Seconds, BOOL Notify=true);

private:
	// Con-/Destructors
	TimeSpan(Handle<String> Name, UINT Seconds): m_Name(Name), m_Value(Seconds) {}

	// Common
	Handle<String> m_Name;
	UINT m_Value;
};

}