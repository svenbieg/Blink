//==========
// Clocks.h
//==========

#pragma once


//=======
// Using
//=======

#include "Devices/IoHelper.h"
#include <base.h>


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
	static const UINT PLL_SYS_HZ	=1'500'000'000; // 1.5GHz
	static const UINT PLL_USB_HZ	=1'200'000'000; // 1.2GHz
	static const UINT SYS_CLK_HZ	=150'000'000; // 150MHz
	static const UINT USB_CLK_HZ	=48'000'000; // 48MHz
	static const UINT XOSC_HZ		=12'000'000; // 12MHz

	static const UINT PERI_CLK_HZ	=SYS_CLK_HZ; // 125MHz
	static const UINT REF_CLK_HZ	=XOSC_HZ; // 12MHz

	// Common
	static VOID Initialize();

private:
	// Common
	static VOID InitializeClock(UINT Id, UINT Source, UINT SrcFreq, UINT Div);
	static VOID InitializePll(SIZE_T Address, UINT RefDiv, UINT VcoFreq, UINT PostDiv1, UINT PostDiv2);
};

}}
