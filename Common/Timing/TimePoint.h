//=============
// TimePoint.h
//=============

#pragma once


//=======
// Using
//=======

#include "Culture/LanguageCode.h"
#include "TimeSpan.h"


//===========
// Namespace
//===========

namespace Timing {


//======================
// Forward-Declarations
//======================

class Clock;


//========
// Struct
//========

class TIMEPOINT
{
public:
	// Con-/Destructors
	TIMEPOINT(): Value(0) {}
	TIMEPOINT(UINT64 Value): Value(Value) {}
	TIMEPOINT(TIMEPOINT const& Value): Value(Value.Value) {}

	// Access
	inline operator BOOL() { return Value!=0; }

	// Comparison
	inline BOOL operator==(TIMEPOINT const& Value) { return this->Value==Value.Value; }
	inline BOOL operator!=(TIMEPOINT const& Value) { return this->Value!=Value.Value; }
	inline BOOL operator>(TIMEPOINT const& Value) { return this->Value>Value.Value; }
	inline BOOL operator>=(TIMEPOINT const& Value) { return this->Value>=Value.Value; }
	inline BOOL operator<(TIMEPOINT const& Value) { return this->Value<Value.Value; }
	inline BOOL operator<=(TIMEPOINT const& Value) { return this->Value<=Value.Value; }

	// Common
	union
		{
		struct
			{
			BYTE Second;
			BYTE Minute;
			BYTE Hour;
			BYTE DayOfWeek;
			BYTE DayOfMonth;
			BYTE Month;
			WORD Year;
			};
		UINT64 Value;
		};
};


//=============
// Time-Format
//=============

enum class TimeFormat
{
Full,
DateTime,
Time
};


//============
// Time-Point
//============

class TimePoint: public Variable
{
public:
	// Con-/Destructors
	static Handle<TimePoint> Create();
	static Handle<TimePoint> Create(TIMEPOINT const& TimePoint);
	static Handle<TimePoint> Create(Handle<String> Name);
	static Handle<TimePoint> Create(Handle<String> Name, TIMEPOINT const& TimePoint);

	// Access
	TIMEPOINT Get();
	static inline TIMEPOINT Get(TimePoint* Value) { return Value? Value->Get(): TIMEPOINT(); }
	static UINT GetDayOfWeek(LPCSTR String);
	static UINT GetDayOfYear(TIMEPOINT const& TimePoint);
	inline Handle<String> GetName()const override { return m_Name; }
	static UINT GetMonth(LPCSTR String);
	inline BOOL IsAbsolute() { return Get().Year!=0; }
	Event<TimePoint, TIMEPOINT&> Reading;
	inline UINT64 ToSeconds() { return ToSeconds(Get()); }
	static UINT64 ToSeconds(TIMEPOINT const& TimePoint);
	Handle<String> ToString(LanguageCode Language=LanguageCode::None)override;
	Handle<String> ToString(TimeFormat Format, LanguageCode Language=LanguageCode::None);
	static Handle<String> ToString(TIMEPOINT const& TimePoint, TimeFormat Format, LanguageCode Language=LanguageCode::None);
	static UINT ToString(TIMEPOINT const& TimePoint, LPSTR Buffer, UINT Size, TimeFormat Format, LanguageCode Language=LanguageCode::None);
	SIZE_T WriteToStream(OutputStream* Stream)override;

	// Modification
	VOID Clear(BOOL Notify=true);
	static VOID FromSeconds(TIMEPOINT* TimePoint, UINT64 Seconds);
	static BOOL FromTimeStamp(TIMEPOINT* TimePoint, LPCSTR TimeStamp);
	SIZE_T ReadFromStream(InputStream* Stream, BOOL Notify=true)override;
	BOOL Set(TIMEPOINT const& TimePoint, BOOL Notify=true);

private:
	// Con-/Destructors
	TimePoint(Handle<String> Name, TIMEPOINT const& Value);

	// Common
	UINT64 GetTickCount(TIMEPOINT const& TimePoint);
	VOID OnClockSecond(Clock* Clock);
	static UINT ToStringDateTime(TIMEPOINT const& TimePoint, LPSTR Buffer, UINT Size, LanguageCode Language);
	static UINT ToStringFull(TIMEPOINT const& TimePoint, LPSTR Buffer, UINT Size, LanguageCode Language);
	static UINT ToStringRelative(UINT64 TickCount, LPSTR Buffer, UINT Size, TimeFormat Format, LanguageCode Language);
	static UINT ToStringTime(TIMEPOINT const& TimePoint, LPSTR Buffer, UINT Size, LanguageCode Language);
	VOID UpdateClock();
	Handle<Clock> m_Clock;
	Handle<String> m_Name;
	TIMEPOINT m_Value;
};

}


//========
// Handle
//========

template <>
class Handle<Timing::TimePoint>
{
public:
	// Friends
	template <class _friend_t> friend class Handle;

	// Using
	using TimePoint=Timing::TimePoint;
	using TIMEPOINT=Timing::TIMEPOINT;

	// Con-/Destructors
	inline Handle(): m_Object(nullptr) {}
	inline Handle(nullptr_t): m_Object(nullptr) {}
	inline Handle(TimePoint* Copy) { Handle<Object>::Create(&m_Object, Copy); }
	inline Handle(Handle const& Copy): Handle(Copy.m_Object) {}
	inline Handle(Handle&& Move)noexcept: m_Object(Move.m_Object) { Move.m_Object=nullptr; }
	inline ~Handle() { Handle<Object>::Clear(&m_Object); }

	// Access
	inline operator BOOL()const { return m_Object&&m_Object->Get(); }
	inline operator TimePoint*()const { return m_Object; }
	inline TimePoint* operator->()const { return m_Object; }

	// Comparison
	inline BOOL operator==(nullptr_t)const { return TimePoint::Get(m_Object).Value==0; }
	inline BOOL operator==(TimePoint* Value)const { return TimePoint::Get(m_Object)==TimePoint::Get(Value); }
	inline BOOL operator==(TIMEPOINT const& Value)const { return TimePoint::Get(m_Object)==Value; }
	inline BOOL operator!=(nullptr_t)const { return TimePoint::Get(m_Object).Value!=0; }
	inline BOOL operator!=(TimePoint* Value)const { return TimePoint::Get(m_Object)!=TimePoint::Get(Value); }
	inline BOOL operator!=(TIMEPOINT const& Value)const { return TimePoint::Get(m_Object)!=Value; }
	inline BOOL operator>(nullptr_t)const { return TimePoint::Get(m_Object).Value>0; }
	inline BOOL operator>(TimePoint* Value)const { return TimePoint::Get(m_Object)>TimePoint::Get(Value); }
	inline BOOL operator>(TIMEPOINT const& Value)const { return TimePoint::Get(m_Object)>Value; }
	inline BOOL operator>=(nullptr_t)const { return TimePoint::Get(m_Object).Value>=0; }
	inline BOOL operator>=(TimePoint* Value)const { return TimePoint::Get(m_Object)>=TimePoint::Get(Value); }
	inline BOOL operator>=(TIMEPOINT const& Value)const { return TimePoint::Get(m_Object)>=Value; }
	inline BOOL operator<(nullptr_t)const { return false; }
	inline BOOL operator<(TimePoint* Value)const { return TimePoint::Get(m_Object)<TimePoint::Get(Value); }
	inline BOOL operator<(TIMEPOINT const& Value)const { return TimePoint::Get(m_Object)<Value; }
	inline BOOL operator<=(nullptr_t)const { return TimePoint::Get(m_Object).Value<=0; }
	inline BOOL operator<=(TimePoint* Value)const { return TimePoint::Get(m_Object)<=TimePoint::Get(Value); }
	inline BOOL operator<=(TIMEPOINT const& Value)const { return TimePoint::Get(m_Object)<=Value; }

	// Assignment
	inline Handle& operator=(nullptr_t) { Handle<Object>::Clear(&m_Object); return *this; }
	inline Handle& operator=(TimePoint* Copy) { Handle<Object>::Set(&m_Object, Copy); return *this; }
	inline Handle& operator=(Handle const& Copy) { return operator=(Copy.m_Object); }
	Handle& operator=(TIMEPOINT const& Value)
		{
		if(!m_Object)
			{
			auto value=TimePoint::Create(Value);
			return operator=(value);
			}
		m_Object->Set(Value);
		return *this;
		}

private:
	// Common
	TimePoint* m_Object;
};


//==================
// Con-/Destructors
//==================

namespace Timing {

inline Handle<TimePoint> TimePoint::Create()
{
TIMEPOINT tp={ 0 };
return new TimePoint(nullptr, tp);
}

inline Handle<TimePoint> TimePoint::Create(TIMEPOINT const& Value)
{
return new TimePoint(nullptr, Value);
}

inline Handle<TimePoint> TimePoint::Create(Handle<String> Name)
{
TIMEPOINT tp={ 0 };
return new TimePoint(Name, tp);
}

inline Handle<TimePoint> TimePoint::Create(Handle<String> Name, TIMEPOINT const& Value)
{
return new TimePoint(Name, Value);
}

}