//========
// init.S
//========

.section ".reset"

#include <base.h>
#include <config.h>
#include <cpu.h>


//===============
// Reset-Handler
//===============

.globl isr_reset
isr_reset:
cpsid i
cpu_id r0

_setup_stack:
ldr r1, =CONFIG_STACK_SIZE
mul r1, r1, r0
ldr r2, =__stack_end
sub r2, r2, r1
mov sp, r2
msr psp, r2
ldr r1, =CONFIG_STACK_SIZE
sub r2, r2, r1
msr msplim, r2
msr psplim, r2

_init_coproc:
#define CPACR 0xED88
#define CPACR_DCP (0x3<<(2*4))
#define CPACR_GPIO (0x3<<(2*0))
ldr r0, =PPB_BASE+CPACR
ldr r1, [r0]
ldr r2, =(CPACR_DCP|CPACR_GPIO)
orr r1, r1, r2
str r1, [r0]
dsb
isb
cdp p4,#0,c0,c0,c0,#0

_start:
cpu_id r0
cbz r0, 1f
ldr r0, =start_secondary
bx r0
1:
ldr r0, =start
bx r0
