//=======
// irq.S
//=======

.section ".text"


//=======
// Using
//=======

#include "Runtime/Configuration.h"
#include "Runtime/IrqFrame.h"
#include "Runtime/TaskFrame.h"
#include "cpu.inc.S"


//=======
// Stubs
//=======

.macro isr_stub name
.weak \name
.thumb_func
\name:
bkpt #0
b \name
.endm

isr_stub isr_reset
isr_stub isr_nmi
isr_stub isr_hardfault
isr_stub isr_pagefault
isr_stub isr_busfault
isr_stub isr_usagefault
isr_stub isr_securefault
isr_stub isr_svcall
isr_stub isr_debugmon
isr_stub isr_systick
isr_stub isr_invalid

.altmacro
.macro irq_stub irq_num
.weak isr_irq\irq_num
.thumb_func
isr_irq\irq_num:
mov r0, #\irq_num
ldr r1, =irq_handler
bx r1
.endm

.set irq_num, 0
.rept CONFIG_IRQ_COUNT
irq_stub %irq_num
.set irq_num, irq_num + 1
.endr


//===========
// IRQ-Stack
//===========

.macro irq_stack_init
cpu_id r1
ldr r2, =CONFIG_STACK_SIZE
mul r1, r1, r2
ldr r2, =__stack_end
sub r2, r2, r1
mov r1, sp
mov sp, r2
sub sp, sp, #IRQ_STACK_SIZE
str r1, [sp, #IRQ_STACK_SP]
mov r1, lr
str r1, [sp, #IRQ_STACK_LR]
.endm

.macro irq_stack_restore
ldr r0, [sp, #IRQ_STACK_SP]
ldr r1, [sp, #IRQ_STACK_LR]
mov sp, r0
mov lr, r1
.endm


//===================
// Interrupt-Handler
//===================

globl_func irq_handler
cpsid i
irq_stack_init
bl HandleInterrupt
irq_stack_restore
cpsie i
bx lr


//=============
// Task-Switch
//=============

globl_func isr_pendsv
cpsid i
irq_stack_init
bl HandleTaskSwitch
irq_stack_restore
cpsie i
bx lr
