//========
// init.S
//========

.section ".text.init"


//=======
// Using
//=======

#include "config.h"
#include "cpu.inc.S"


//=============
// Entry-Point
//=============

.globl init
init:


//===================
// Exception Level 2
//===================

_check_el1:
#define EL1 0x4
#define EL2 0x8
#define EL3 0xC
mrs x0, CurrentEL
cmp x0, #EL1
beq _init_exceptions_el1


//=======
// Timer
//=======

_init_timer_el2:
#define CNTHCTL_EL1PCEN (1<<1)
#define CNTHCTL_EL1PCTEN (1<<0)
mov x0, #(CNTHCTL_EL1PCEN|CNTHCTL_EL1PCTEN) // Enable EL1 access
msr CNTHCTL_EL2, x0
msr CNTVOFF_EL2, xzr


//===================
// Exception Level 1
//===================

_init_exceptions_el2:
ldr x0, =_vectors_el2
msr VBAR_EL2, x0
_switch_to_el1:
#define SPSR_D (1<<9) // Debug exception mask
#define SPSR_A (1<<8) // SError interrupt mask
#define SPSR_I (1<<7) // IRQ interrupt mask
#define SPSR_F (1<<6) // FIQ interrupt mask
#define SPSR_M (5<<0) // EL1H Mode
ldr x0, =(SPSR_D|SPSR_A|SPSR_I|SPSR_F|SPSR_M)
msr SPSR_EL2, x0
adr x0, 1f
msr ELR_EL2, x0
eret
1:


//============
// Exceptions
//============

_init_exceptions_el1:
ldr x0, =_vectors_el1
msr VBAR_EL1, x0


//===============
// Co-Processors
//===============

_init_coproc:
#define CPACR_FPEN (3<<20) // Enable FPU
mov x0, #CPACR_FPEN
msr CPACR_EL1, x0


//=======
// Stack
//=======

_init_stack:
cpu_id x0
mov x1, #CONFIG_STACK_SIZE
mul x1, x1, x0
ldr x2, =__stack_end
sub x2, x2, x1
mov sp, x2


//=======
// Start
//=======

_start:
cpu_id x0
cbnz x0, _start_secondary
b start
_start_secondary:
b start_secondary
