//============
// Clocks.cpp
//============

#include "Clocks.h"


//=======
// Using
//=======

#include "Devices/System/System.h"
#include "Devices/IoHelper.h"
#include <base.h>

using namespace Devices::System;


//===========
// Namespace
//===========

namespace Devices {
	namespace Timers {


//========
// Clocks
//========

enum CLOCK
{
CLK_GPOUT0,
CLK_GPOUT1,
CLK_GPOUT2,
CLK_GPOUT3,
CLK_REF,
CLK_SYS,
CLK_PERI,
CLK_HSTX,
CLK_USB,
CLK_ADC
};


//===========
// Registers
//===========

typedef struct
{
RW32 CTRL;
RW32 DIV;
RO32 SEL;
}CLOCK_REGS;

typedef struct
{
CLOCK_REGS CLOCK[10];
RW32 DFTCLK_XOSC_CTRL;
RW32 DFTCLK_ROSC_CTRL;
RW32 DFTCLK_LPOSC_CTRL;
RW32 RESUS_CTRL;
RO32 RESUS_STATUS;
RW32 FC_REF_KHZ;
RW32 FC_MIN_KHZ;
RW32 FC_MAX_KHZ;
RW32 FC_DELAY;
RW32 FC_INTERVAL;
RW32 FC_SOURCE;
RO32 FC_STATUS;
RO32 FC_RESULT;
RW32 WAKE_EN0;
RW32 WAKE_EN1;
RW32 SLEEP_EN0;
RW32 SLEEP_EN1;
RW32 EN0;
RW32 EN1;
RO32 INTR;
RW32 INTE;
RW32 INTF;
RO32 INTS;
}CLOCKS_REGS;

const UINT32 CLK_CTRL_EN		=(1<<11);
const BITS32 CLK_CTRL_SRC		={ 0x3, 0 };

const BITS32 CLK_DIV_INT		={ 0xFFFF, 16 };

enum CLK_SRC
{
CLK_SRC_SYS_AUX=1,
CLK_SRC_XOSC=2
};


//====================
// Crystal-Oscillator
//====================

const UINT XOSC_HZ=12'000'000;

typedef struct
{
RW32 CTRL;
RW32 STATUS;
RW32 DORMANT;
RW32 STARTUP;
RW32 COUNT;
}XOSC_REGS;

const UINT XOSC_CTRL_1_15MHZ=0xAA0;
const UINT XOSC_CTRL_EN=0xFAB<<12;

const UINT XOSC_STATUS_STABLE=(1<<31);

const UINT XOSC_STARTUP_DELAY=(XOSC_HZ/1000+128)/256*6;


//=====
// Pll
//=====

typedef struct
{
RW32 CS;
RW32 PWR;
RW32 FBDIV;
RW32 PRIM;
RW32 INTR;
RW32 INTE;
RW32 INTF;
RO32 INTS;
}PLL_REGS;

const UINT32 PLL_CS_LOCKED			=(1<<31);
const BITS32 PLL_PRIM_POSTDIV1		={ 0x7, 16 };
const BITS32 PLL_PRIM_POSTDIV2		={ 0x7, 12 };

const UINT PLL_PWR_VCOPD			=(1<<5);
const UINT PLL_PWR_POSTDIVPD		=(1<<3);
const UINT PLL_PWR_MAINPD			=(1<<0);


//========
// Ticker
//========

enum TICKER
{
TICKER_PROC0,
TICKER_PROC1,
TICKER_TIMER0,
TICKER_TIMER1,
TICKER_WATCHDOG,
TICKER_RISCV,
TICKER_COUNT
};

typedef struct
{
RW32 CTRL;
RW32 CYCLES;
RO32 COUNT;
}TICKER_REGS;

const UINT TICKER_CTRL_EN=(1<<0);
const UINT TICKER_CYCLES=Clocks::REF_CLK_HZ/1'000'000;

typedef struct
{
TICKER_REGS TICKER[TICKER_COUNT];
}TICKS_REGS;


//========
// Common
//========

VOID Clocks::Initialize()
{
using System=Devices::System::System;
auto clocks=(CLOCKS_REGS*)CLOCKS_BASE;
IoHelper::Write(clocks->RESUS_CTRL, 0);
auto xosc=(XOSC_REGS*)XOSC_BASE;
IoHelper::Write(xosc->CTRL, XOSC_CTRL_1_15MHZ);
IoHelper::Write(xosc->STARTUP, XOSC_STARTUP_DELAY);
IoHelper::Set(xosc->CTRL, XOSC_CTRL_EN);
IoHelper::Retry(xosc->STATUS, XOSC_STATUS_STABLE, XOSC_STATUS_STABLE);
System::Enable(ResetDevice::PllSys, 0);
System::Enable(ResetDevice::PllUsb, 0);
InitializePll(PLL_SYS_BASE, 1, PLL_SYS_HZ, 5, 2);
InitializePll(PLL_USB_BASE, 1, PLL_USB_HZ, 5, 5);
InitializeClock(CLK_REF, CLK_SRC_XOSC, XOSC_HZ, 1);
InitializeClock(CLK_SYS, CLK_SRC_SYS_AUX, SYS_CLK_HZ, 1);
InitializeClock(CLK_PERI, 0, SYS_CLK_HZ, 1);
InitializeClock(CLK_HSTX, 0, SYS_CLK_HZ, 1);
InitializeClock(CLK_USB, 0, USB_CLK_HZ, 1);
InitializeClock(CLK_ADC, 0, USB_CLK_HZ, 1);
auto ticks=(TICKS_REGS*)TICKS_BASE;
for(UINT u=0; u<TICKER_COUNT; u++)
	{
	ticks->TICKER[u].CYCLES=TICKER_CYCLES;
	ticks->TICKER[u].CTRL=TICKER_CTRL_EN;
	}
}


//================
// Common Private
//================

VOID Clocks::InitializeClock(UINT id, UINT src, UINT src_hz, UINT div)
{
auto clocks=(CLOCKS_REGS*)CLOCKS_BASE;
auto clock=&clocks->CLOCK[id];
IoHelper::Set(clock->CTRL, CLK_CTRL_SRC, src);
UINT mask=1U<<src;
IoHelper::Retry(clock->SEL, mask, mask);
IoHelper::Set(clock->CTRL, CLK_CTRL_EN);
}

VOID Clocks::InitializePll(SIZE_T addr, UINT ref_div, UINT vco_hz, UINT pdiv1, UINT pdiv2)
{
UINT ref_hz=XOSC_HZ/ref_div;
UINT fbdiv=vco_hz/ref_hz;
auto pll=(PLL_REGS*)addr;
IoHelper::Set(pll->PWR, PLL_PWR_POSTDIVPD);
IoHelper::Write(pll->CS, ref_div);
IoHelper::Write(pll->FBDIV, fbdiv);
IoHelper::Clear(pll->PWR, PLL_PWR_MAINPD|PLL_PWR_VCOPD);
IoHelper::Retry(pll->CS, PLL_CS_LOCKED, PLL_CS_LOCKED, 100);
UINT prim=0;
BitHelper::Set(prim, PLL_PRIM_POSTDIV1, pdiv1);
BitHelper::Set(prim, PLL_PRIM_POSTDIV2, pdiv2);
IoHelper::Write(pll->PRIM, prim);
IoHelper::Clear(pll->PWR, PLL_PWR_POSTDIVPD);
}

}}