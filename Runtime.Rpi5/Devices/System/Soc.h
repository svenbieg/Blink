//=======
// Soc.h
//=======

#pragma once


//=======
// Using
//=======

#include <io.h>


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
rw32_t RES;
rw32_t STEPPING;
}soc_regs_t;

// STEPPING
constexpr bits32_t SOC_STEPPING={ 0xFF, 0 };

}}