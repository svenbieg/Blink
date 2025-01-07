//============
// Language.h
//============

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


//===============
// Language-Code
//===============

constexpr STRING STR_LANGUAGE_CODE[]=
	{
	{ LNG::Neutral, L"*" },
	{ LNG::DE, L"DE" },
	{ LNG::EN, L"EN" },
	{ LNG::None, nullptr }
	};


//===========
// Languages
//===========

constexpr STRING STR_ENGLISH[]=
	{
	{ LNG::DE, L"Englisch" },
	{ LNG::EN, L"English" },
	{ LNG::None, nullptr }
	};

constexpr STRING STR_GERMAN[]=
	{
	{ LNG::DE, L"Deutsch" },
	{ LNG::EN, L"German" },
	{ LNG::None, nullptr }
	};

constexpr STRING STR_NEUTRAL[]=
	{
	{ LNG::DE, L"Neutral" },
	{ LNG::EN, L"Neutral" },
	{ LNG::None, nullptr }
	};


//=============
// Translation
//=============

typedef struct
{
LNG Language;
STRING const* String;
}TRANSLATION;

constexpr TRANSLATION TRANS_LANGUAGE[]=
	{
	{ LNG::Neutral, STR_NEUTRAL },
	{ LNG::DE, STR_GERMAN },
	{ LNG::EN, STR_ENGLISH },
	{ LNG::None, nullptr }
	};

}}