//===========
// cpu.inc.S
//===========

#include "config.h"


//========
// Macros
//========

#ifdef __ASSEMBLER__

.macro cpu_id reg
mrs \reg, MPIDR_EL1
lsr \reg, \reg, #8
and \reg, \reg, #CONFIG_CPU_COUNT-1
.endm

.macro cpu_on core
#define PSCI_CPU_ON 0xC4000003
ldr x0, =PSCI_CPU_ON
mov x1, #(\core<<8)
ldr x2, =init_secondary
smc #0
.endm

#endif
