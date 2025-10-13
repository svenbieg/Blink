//=========
// start.S
//=========

.section ".text.start"


//=======
// Using
//=======

#include <cpu.h>


//=============
// Entry-Point
//=============

.globl _start
_start:


//============
// Exceptions
//============

_init_exceptions:
ldr x0, =_vectors_el2
msr VBAR_EL2, x0
ldr x0, =_vectors_el1
msr VBAR_EL1, x0


//=======
// Timer
//=======

_init_timer:
#define CNTHCTL_EL1PCEN (1<<1)
#define CNTHCTL_EL1PCTEN (1<<0)
mov x0, #(CNTHCTL_EL1PCEN|CNTHCTL_EL1PCTEN) // Enable EL1 access
msr CNTHCTL_EL2, x0
msr CNTVOFF_EL2, xzr


//===============
// Co-Processors
//===============

_init_fpu:
#define CPACR_FPEN (3<<20) // Enable FPU at EL1
mov x0, #CPACR_FPEN
msr CPACR_EL1, x0


//===================
// Exception Level 1
//===================

_switch_to_el1:
mrs x0, CurrentEL
cmp x0, #4
b.eq 1f
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
// Enable MMU
//============

_enable_mmu:
cpu_id x0
cbnz x0, 1f
bl mmu_create_page_table
1:
bl mmu_enable


//=======
// Stack
//=======

_init_stack:
cpu_id x0
mov x1, #STACK_SIZE
mul x1, x1, x0
ldr x2, =__stack_end
sub x2, x2, x1
mov sp, x2


//=======
// Start
//=======

_start_cpp:
cpu_id x0
cbz x0, 1f
bl start_secondary
b _shutdown
1:
bl start


//==========
// Shutdown
//==========

_shutdown:
#define PSCI_SHUTDOWN 0xC4000008
ldr x0, =PSCI_SHUTDOWN
smc #0
1:
wfe
b 1b
