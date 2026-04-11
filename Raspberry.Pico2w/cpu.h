//=======
// cpu.h
//=======

#pragma once


//========
// Macros
//========

#ifdef __ASSEMBLER__

#define SIO_BASE 0xD0000000
#define SIO_CPUID 0

.macro cpu_id reg
ldr \reg, =(SIO_BASE+SIO_CPUID)
ldr \reg, [\reg]
.endm

#endif
