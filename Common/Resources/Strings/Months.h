//==========
// Months.h
//==========

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


//========
// Months
//========

constexpr STRING STR_JANUARY[]=
	{
	{ LNG::DE, L"Jan" },
	{ LNG::EN, L"Jan" },
	{ LNG::None, nullptr }
	};

constexpr STRING STR_FEBRUARY[]=
	{
	{ LNG::DE, L"Feb" },
	{ LNG::EN, L"Feb" },
	{ LNG::None, nullptr }
	};

constexpr STRING STR_MARCH[]=
	{
	{ LNG::DE, L"Mär" },
	{ LNG::EN, L"Mar" },
	{ LNG::None, nullptr }
	};

constexpr STRING STR_APRIL[]=
	{
	{ LNG::DE, L"Apr" },
	{ LNG::EN, L"Apr" },
	{ LNG::None, nullptr }
	};

constexpr STRING STR_MAY[]=
	{
	{ LNG::DE, L"Mai" },
	{ LNG::EN, L"May" },
	{ LNG::None, nullptr }
	};

constexpr STRING STR_JUNE[]=
	{
	{ LNG::DE, L"Jun" },
	{ LNG::EN, L"Jun" },
	{ LNG::None, nullptr }
	};

constexpr STRING STR_JULY[]=
	{
	{ LNG::DE, L"Jul" },
	{ LNG::EN, L"Jul" },
	{ LNG::None, nullptr }
	};

constexpr STRING STR_AUGUST[]=
	{
	{ LNG::DE, L"Aug" },
	{ LNG::EN, L"Aug" },
	{ LNG::None, nullptr }
	};

constexpr STRING STR_SEPTEMBER[]=
	{
	{ LNG::DE, L"Sep" },
	{ LNG::EN, L"Sep" },
	{ LNG::None, nullptr }
	};

constexpr STRING STR_OCTOBER[]=
	{
	{ LNG::DE, L"Okt" },
	{ LNG::EN, L"Oct" },
	{ LNG::None, nullptr }
	};

constexpr STRING STR_NOVEMBER[]=
	{
	{ LNG::DE, L"Nov" },
	{ LNG::EN, L"Nov" },
	{ LNG::None, nullptr }
	};

constexpr STRING STR_DECEMBER[]=
	{
	{ LNG::DE, L"Dez" },
	{ LNG::EN, L"Dec" },
	{ LNG::None, nullptr }
	};

constexpr STRING const* STRS_MONTHS[]=
	{
	STR_JANUARY,
	STR_FEBRUARY,
	STR_MARCH,
	STR_APRIL,
	STR_MAY,
	STR_JUNE,
	STR_JULY,
	STR_AUGUST,
	STR_SEPTEMBER,
	STR_OCTOBER,
	STR_NOVEMBER,
	STR_DECEMBER,
	nullptr
	};

}}