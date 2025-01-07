//========
// Days.h
//========

#pragma once


//=======
// Using
//=======

#include "Resources/Strings/String.h"


//===========
// Namespace
//===========

namespace Resources {
	namespace Strings {


//======
// Days
//======

constexpr STRING STR_MONDAY[]=
	{
	{ LNG::DE, L"Mo" },
	{ LNG::EN, L"Mon" },
	{ LNG::None, nullptr }
	};

constexpr STRING STR_TUESDAY[]=
	{
	{ LNG::DE, L"Di" },
	{ LNG::EN, L"Tue" },
	{ LNG::None, nullptr }
	};

constexpr STRING STR_WEDNESDAY[]=
	{
	{ LNG::DE, L"Mi" },
	{ LNG::EN, L"Wed" },
	{ LNG::None, nullptr }
	};

constexpr STRING STR_THURSDAY[]=
	{
	{ LNG::DE, L"Do" },
	{ LNG::EN, L"Thu" },
	{ LNG::None, nullptr }
	};

constexpr STRING STR_FRIDAY[]=
	{
	{ LNG::DE, L"Fr" },
	{ LNG::EN, L"Fri" },
	{ LNG::None, nullptr }
	};

constexpr STRING STR_SATURDAY[]=
	{
	{ LNG::DE, L"Sa" },
	{ LNG::EN, L"Sat" },
	{ LNG::None, nullptr }
	};

constexpr STRING STR_SUNDAY[]=
	{
	{ LNG::DE, L"So" },
	{ LNG::EN, L"Sun" },
	{ LNG::None, nullptr }
	};

constexpr STRING const* STRS_DAYS[]=
	{
	STR_MONDAY,
	STR_TUESDAY,
	STR_WEDNESDAY,
	STR_THURSDAY,
	STR_FRIDAY,
	STR_SATURDAY,
	STR_SUNDAY,
	nullptr
	};

}}