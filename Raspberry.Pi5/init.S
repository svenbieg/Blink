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


//=======
// Timer
//=======

_init_timer:
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


//==================
// Wakeup secondary
//==================

_wakeup_secondary:
//cpu_on 1
//cpu_on 2
//cpu_on 3
b _init_exceptions_el1


//===========
// Secondary
//===========

.globl init_secondary
init_secondary:
wfe
b init_secondary


//============
// Exceptions
//============

_init_exceptions_el1:
ldr x0, =_vectors_el1
msr VBAR_EL1, x0


//========
// Memory
//========

_enable_mmu:
ldr x0, =__page_table
msr TTBR0_EL1, x0
#define MAIR_ATT_CACHED (0xFF<<(MMU_ATT_CACHED*8))
#define MAIR_ATT_UNCACHED (0x44<<(MMU_ATT_UNCACHED*8))
#define MAIR_ATT_DEVICE (0x04<<(MMU_ATT_DEVICE*8))
ldr x0, =(MAIR_ATT_CACHED|MAIR_ATT_UNCACHED|MAIR_ATT_DEVICE)
msr MAIR_EL1, x0
#define TCR_IPS_4GB (0ULL<<32)
#define TCR_IPS_1TB (2ULL<<32)
#define TCR_EPD1 (1<<23)
#define TCR_TG0_4KB (0<<14)
#define TCR_TG0_64KB (1<<14)
#define TCR_SH0_INNER (3<<12)
#define TCR_ORGN0_WB_WA (1<<10)
#define TCR_IRGN0_WB_WA (1<<8)
#define TCR_T0SZ_128GB (27<<0)
ldr x0, =(TCR_IPS_1TB|TCR_EPD1|TCR_TG0_4KB|TCR_SH0_INNER|TCR_ORGN0_WB_WA|TCR_IRGN0_WB_WA|TCR_T0SZ_128GB)
msr TCR_EL1, x0
isb
tlbi vmalle1
dsb sy
isb
#define SCTLR_I (1<<12)
#define SCTLR_C (1<<2)
#define SCTLR_M (1<<0)
mrs x0, SCTLR_EL1
ldr x1, =(SCTLR_I|SCTLR_C|SCTLR_M)
orr x0, x0, x1
msr SCTLR_EL1, x0
isb


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
