//===========
// cpu.inc.S
//===========

#pragma once


//=======
// Using
//=======

#include "Devices/Peripherals.h"


//========
// Macros
//========

#ifdef __ASSEMBLER__

#define SIO_CPUID 0

.macro cpu_id reg
ldr \reg, =(SIO_BASE+SIO_CPUID)
ldr \reg, [\reg]
.endm

#endif
