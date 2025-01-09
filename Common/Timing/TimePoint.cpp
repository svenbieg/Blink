//===============
// TimePoint.cpp
//===============

#include "pch.h"


//=======
// Using
//=======

#include "Culture/Sentence.h"
#include "Devices/Timers/SystemTimer.h"
#include "Resources/Strings/Days.h"
#include "Resources/Strings/Months.h"
#include "Clock.h"
#include "TimePoint.h"

using namespace Culture;
using namespace Devices::Timers;
using namespace Resources::Strings;


//===========
// Namespace
//===========

namespace Timing {


//=========
// Globals
//=========

constexpr WORD DaysInMonth[4][12]=
	{
	{ 0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335 },
	{ 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334 },
	{ 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334 },
	{ 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334 }
	};

constexpr UINT SecondsPerDay=86'400;
constexpr UINT SecondsPerHour=3'600;
constexpr UINT SecondsPerMinute=60;


//========
// Access
//========

TIMEPOINT TimePoint::Get()
{
TIMEPOINT tp=m_Value;
Reading(this, tp);
return tp;
}

UINT TimePoint::GetDayOfWeek(LPCSTR str)
{
auto days=STRS_DAYS;
for(UINT u=0; u<7; u++)
	{
	auto day=Sentence::Translate(days[u], LNG::EN);
	if(StringHelper::Compare(str, day, 3, false)==0)
		return u+1;
	}
return 0;
}

UINT TimePoint::GetDayOfYear(TIMEPOINT const& tp)
{
if(tp.Year==0)
	return 0;
UINT year4=tp.Year%4;
UINT day=DaysInMonth[year4][tp.Month-1];
return day+tp.DayOfMonth-1;
}

UINT TimePoint::GetMonth(LPCSTR str)
{
auto months=STRS_MONTHS;
for(UINT u=0; u<12; u++)
	{
	auto month=Sentence::Translate(months[u], LNG::EN);
	if(StringHelper::Compare(str, month, 3, false)==0)
		return u+1;
	}
return 0;
}

UINT64 TimePoint::ToSeconds(TIMEPOINT const& tp)
{
if(tp.Year==0)
	return 0;
UINT year4=tp.Year%4;
UINT year=tp.Year-year4;
UINT days=year*365+year/4;
constexpr WORD days_per_year[]={ 0, 366, 365, 365 };
days+=days_per_year[year4];
days+=DaysInMonth[year4][tp.Month-1];
UINT64 sec=days*24*60*60;
sec+=tp.Hour*60*60;
sec+=tp.Minute*60;
sec+=tp.Second;
return sec;
}

Handle<String> TimePoint::ToString(LanguageCode lng)
{
CHAR str[64];
ToString(m_Value, str, 64, TimeFormat::DateTime, lng);
return str;
}

Handle<String> TimePoint::ToString(TimeFormat fmt, LanguageCode lng)
{
CHAR str[64];
ToString(m_Value, str, 64, fmt, lng);
return str;
}

Handle<String> TimePoint::ToString(TIMEPOINT const& tp, TimeFormat fmt, LanguageCode lng)
{
CHAR str[64];
ToString(tp, str, 64, fmt, lng);
return str;
}

UINT TimePoint::ToString(TIMEPOINT const& tp, LPSTR str, UINT size, TimeFormat fmt, LanguageCode lng)
{
if(!str||!size)
	return 0;
str[0]=0;
if(tp.Year==0)
	{
	UINT64 ticks=0;
	MemoryHelper::Copy(&ticks, &tp, sizeof(UINT64));
	if(ticks==0)
		return StringHelper::Copy(str, size, "-");
	return ToStringRelative(ticks, str, size, fmt, lng);
	}
switch(fmt)
	{
	default:
	case TimeFormat::DateTime:
		return ToStringDateTime(tp, str, size, lng);
	case TimeFormat::Full:
		return ToStringFull(tp, str, size, lng);
	case TimeFormat::Time:
		return ToStringTime(tp, str, size, lng);
	}
return 0;
}

SIZE_T TimePoint::WriteToStream(OutputStream* stream)
{
if(!stream)
	return sizeof(TIMEPOINT);
TIMEPOINT tp(m_Value);
Reading(this, tp);
if(tp.Year==0)
	MemoryHelper::Fill(&tp, sizeof(TIMEPOINT), 0);
return stream->Write(&tp, sizeof(TIMEPOINT));
}


//==============
// Modification
//==============

VOID TimePoint::Clear(BOOL notify)
{
TIMEPOINT tp={ 0 };
Set(tp, notify);
}

VOID TimePoint::FromSeconds(TIMEPOINT* tp, UINT64 seconds)
{
constexpr UINT sec_per_year4=126'230'400;
UINT year4=(UINT)(seconds/sec_per_year4);
seconds-=year4*sec_per_year4;
UINT year=year4*4;
constexpr UINT sec_per_year[4]={ 31'622'400, 31'536'000, 31'536'000, 31'536'000 };
for(year4=0; seconds>=sec_per_year[year4]; year4++)
	seconds-=sec_per_year[year4];
year+=year4;
UINT day_of_year=seconds/SecondsPerDay;
seconds-=day_of_year*SecondsPerDay;
BYTE hour=(BYTE)(seconds/SecondsPerHour);
seconds-=hour*SecondsPerHour;
BYTE min=(BYTE)(seconds/SecondsPerMinute);
BYTE sec=(BYTE)(seconds%SecondsPerMinute);
constexpr BYTE days_per_month[4][12]=
	{
	{ 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 },
	{ 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 },
	{ 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 },
	{ 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 }
	};
UINT day=day_of_year;
UINT month=0;
for(; day>=days_per_month[year4][month]; month++)
	day-=days_per_month[year4][month];
constexpr BYTE days_of_week[28]=
	{
	0, 2, 3, 4,
	5, 7, 8, 9,
	10, 12, 13, 14,
	15, 17, 18, 19,
	20, 22, 23, 24,
	25, 27, 28, 29,
	30, 32, 33, 34
	};
UINT year28=year%28;
UINT day_of_week=(days_of_week[year28]+day_of_year)%7;
tp->DayOfMonth=day+1;
tp->DayOfWeek=day_of_week;
tp->Hour=hour;
tp->Minute=min;
tp->Month=month+1;
tp->Second=sec;
tp->Year=year;
}

BOOL TimePoint::FromTimeStamp(TIMEPOINT* tp, LPCSTR str)
{
CHAR day_str[4];
CHAR mon_str[4];
UINT mday=0;
UINT hour=0;
UINT min=0;
UINT sec=0;
UINT year=0;
if(StringHelper::Scan(str, "%s %s %u %u:%u:%u %u", day_str, 4, mon_str, 4, &mday, &hour, &min, &sec, &year)!=7)
	return false;
UINT wday=GetDayOfWeek(day_str);
if(wday==0)
	return false;
UINT mon=GetMonth(mon_str);
if(mon==0)
	return false;
year-=1900;
tp->Second=(BYTE)sec;
tp->Minute=(BYTE)min;
tp->Hour=(BYTE)hour;
tp->DayOfWeek=(BYTE)wday;
tp->DayOfMonth=(BYTE)mday;
tp->Month=(BYTE)mon;
tp->Year=(WORD)year;
return true;
}

SIZE_T TimePoint::ReadFromStream(InputStream* stream, BOOL notify)
{
if(!stream)
	return sizeof(TIMEPOINT);
TIMEPOINT value;
SIZE_T size=stream->Read(&value, sizeof(TIMEPOINT));
if(size==sizeof(TIMEPOINT))
	Set(value, notify);
return size;
}

BOOL TimePoint::Set(TIMEPOINT const& value, BOOL notify)
{
if(m_Value==value)
	return false;
m_Value=value;
if(notify)
	Changed(this);
return true;
}


//==========================
// Con-/Destructors Private
//==========================

TimePoint::TimePoint(Handle<String> name, TIMEPOINT const& value):
m_Name(name),
m_Value(value)
{
UpdateClock();
}


//================
// Common Private
//================

UINT64 TimePoint::GetTickCount(TIMEPOINT const& tp)
{
if(tp.Year!=0)
	return 0;
UINT64 ticks=0;
MemoryHelper::Copy(&ticks, &tp, sizeof(UINT64));
return ticks;
}

VOID TimePoint::OnClockSecond(Clock* clock)
{
if(!Clock::Update(&m_Value))
	return;
clock->Second.Remove(this);
Changed(this);
}

UINT TimePoint::ToStringDateTime(TIMEPOINT const& tp, LPSTR str, UINT size, LanguageCode lng)
{
if(!str||!size)
	return 0;
str[0]=0;
UINT mday=tp.DayOfMonth;
UINT mon=tp.Month+1;
UINT year=tp.Year+1900;
UINT hour=tp.Hour;
UINT min=tp.Minute;
switch(lng)
	{
	case LNG::None:
	case LNG::DE:
		return StringHelper::Print(str, size, "%02u.%02u.%u %02u:%02u", mday, mon, year, hour, min);
	case LNG::EN:
		return StringHelper::Print(str, size, "%02u-%02u-%u %02u:%02u", mon, mday, year, hour, min);
	}
return 0;
}

UINT TimePoint::ToStringFull(TIMEPOINT const& tp, LPSTR str, UINT size, LanguageCode lng)
{
if(!str||!size)
	return 0;
str[0]=0;
UINT year=tp.Year+1900;
UINT mon=tp.Month+1;
UINT mday=tp.DayOfMonth;
UINT wday=tp.DayOfWeek;
UINT hour=tp.Hour;
UINT min=tp.Minute;
if(wday==0||wday>7||mon==0||mon>12)
	return StringHelper::Copy(str, size, "-");
auto str_mon=Sentence::Translate(STRS_MONTHS[mon-1], lng);
auto str_day=Sentence::Translate(STRS_DAYS[wday-1], lng);
switch(lng)
	{
	case LNG::None:
	case LNG::DE:
		return StringHelper::Print(str, size, "%s, %u. %s %u %02u:%02u", str_day, mday, str_mon, year, hour, min);
	case LNG::EN:
		return StringHelper::Print(str, size, "%s, %u %s %u - %02u:%02u", str_day, mday, str_mon, year, hour, min);
	}
return 0;
}

UINT TimePoint::ToStringRelative(UINT64 ticks, LPSTR str, UINT size, TimeFormat fmt, LanguageCode lng)
{
if(!str||!size)
	return 0;
str[0]=0;
UINT64 ticks_now=SystemTimer::GetTickCount();
UINT delta=(UINT)((ticks_now-ticks)/1000);
CHAR time_span[32];
TimeSpan::ToString(time_span, 32, delta);
UINT len=0;
switch(lng)
	{
	case LNG::None:
	case LNG::DE:
		return StringHelper::Print(str, size, "vor %s", time_span);
	case LNG::EN:
		return StringHelper::Print(str, size, "%s ago", time_span);
	}
return 0;
}

UINT TimePoint::ToStringTime(TIMEPOINT const& tp, LPSTR str, UINT size, LanguageCode lng)
{
UINT hour=tp.Hour;
UINT min=tp.Minute;
return StringHelper::Print(str, size, "%02u:%02u", hour, min);
}

VOID TimePoint::UpdateClock()
{
UINT64 ticks=GetTickCount(m_Value);
if(ticks==0)
	{
	if(m_Clock)
		{
		m_Clock->Second.Remove(this);
		m_Clock=nullptr;
		}
	return;
	}
if(!m_Clock)
	{
	m_Clock=Clock::Get();
	m_Clock->Second.Add(this, &TimePoint::OnClockSecond);
	}
}

}