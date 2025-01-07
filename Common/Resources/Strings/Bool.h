//========
// Bool.h
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
// Bool
//======

constexpr STRING STR_FALSE[]=
	{
	{ LNG::Neutral, L"0" },
	{ LNG::DE, L"falsch" },
	{ LNG::DE, L"Nein" },
	{ LNG::EN, L"false" },
	{ LNG::EN, L"no" },
	{ LNG::None, nullptr }
	};

constexpr STRING STR_TRUE[]=
	{
	{ LNG::Neutral, L"1" },
	{ LNG::DE, L"wahr" },
	{ LNG::DE, L"Ja" },
	{ LNG::EN, L"true" },
	{ LNG::EN, L"yes" },
	{ LNG::None, nullptr }
	};

}}