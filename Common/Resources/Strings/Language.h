//============
// Language.h
//============

#pragma once


//=======
// Using
//=======

#include "String.h"


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
	{ LNG::DE, L"DE" },
	{ LNG::EN, L"EN" },
	{ LNG::None, nullptr }
	};


//===========
// Languages
//===========

constexpr STRING STR_GERMAN[]=
	{
	{ LNG::DE, L"Deutsch" },
	{ LNG::EN, L"German" },
	{ LNG::None, nullptr }
	};

constexpr STRING STR_ENGLISH[]=
	{
	{ LNG::DE, L"Englisch" },
	{ LNG::EN, L"English" },
	{ LNG::None, nullptr }
	};

}}