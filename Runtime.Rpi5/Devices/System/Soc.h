//=======
// Soc.h
//=======

#pragma once


//=======
// Using
//=======

#include "Devices/IoHelper.h"


//===========
// Namespace
//===========

namespace Devices {
	namespace System {


//===========
// Registers
//===========

typedef struct
{
RW32 RES;
RW32 STEPPING;
}SOC_REGS;

// STEPPING
constexpr BITS SOC_STEPPING={ 0xFF, 0 };

}}