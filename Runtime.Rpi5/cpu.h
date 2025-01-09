//=======
// cpu.h
//=======

#pragma once


//=======
// Using
//=======

#include <config.h>


//========
// Macros
//========

#ifdef __ASSEMBLER__

.macro cpu_id reg
mrs \reg, MPIDR_EL1
lsr \reg, \reg, #8
and \reg, \reg, #(CPU_COUNT-1)
.endm

#endif
