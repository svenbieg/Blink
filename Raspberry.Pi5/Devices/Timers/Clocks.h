//==========
// Clocks.h
//==========

#pragma once


//=======
// Using
//=======

#include "TypeHelper.h"


//===========
// Namespace
//===========

namespace Devices {
	namespace Timers {


//========
// Clocks
//========

class Clocks
{
public:
	// Settings
	static const UINT SYS_CLK_HZ	=1'500'000'000; // 1.5GHz
	static const UINT XOSC_HZ		=50'000'000; // 50MHz
};

}}
