//==========
// Status.h
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
// Status
//========

constexpr STRING STR_FAILED[]=
	{
	{ LNG::DE, L"Fehlgeschlagen" },
	{ LNG::EN, L"Failed" },
	{ LNG::None, nullptr }
	};

constexpr STRING STR_SUCCESS[]=
	{
	{ LNG::DE, L"Erfolg" },
	{ LNG::EN, L"Success" },
	{ LNG::None, nullptr }
	};

}}