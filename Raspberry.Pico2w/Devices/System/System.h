//==========
// System.h
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
	namespace System {


//=======
// Reset
//=======

// RESET:		0x0D7DF93F
// RESET_DONE:	0x02C206C0

enum class ResetDevice
{
Adc				=(1<<0),
BusCtrl			=(1<<1),
Dma				=(1<<2),
Hstx			=(1<<3),
I2c0			=(1<<4),
I2c1			=(1<<5),
//IoBank0		=(1<<6),
//IoQspi		=(1<<7),
Jtag			=(1<<8),
//PadsBank0		=(1<<9),
//PadsQspi		=(1<<10),
Pio0			=(1<<11),
Pio1			=(1<<12),
Pio2			=(1<<13),
PllSys			=(1<<14),
PllUsb			=(1<<15),
Pwm				=(1<<16),
//Sha256		=(1<<17),
Spi0			=(1<<18),
Spi1			=(1<<19),
SysConf			=(1<<20),
SysInfo			=(1<<21),
TbMan			=(1<<22),
//Timer0		=(1<<23),
Timer1			=(1<<24),
//Trng			=(1<<25),
UArt0			=(1<<26),
UArt1			=(1<<27),
Usb				=(1<<28)
};


//========
// System
//========

class System
{
public:
	// Common
	static VOID Enable(ResetDevice Device, UINT Timeout=100);
	[[noreturn]] static VOID PowerOff();
	[[noreturn]] static VOID Restart();
};

}}