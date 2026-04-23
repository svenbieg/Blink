//=======
// irq.S
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


//=======
// Stack
//=======

.macro init_irq_stack
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

.macro restore_irq_stack
ldr r0, [sp, #IRQ_STACK_SP]
ldr r1, [sp, #IRQ_STACK_LR]
mov sp, r0
mov lr, r1
.endm


//===================
// Interrupt-Handler
//===================

globl_func irq_handler
init_irq_stack
bl HandleInterrupt
restore_irq_stack
bx lr


//=============
// Task-Switch
//=============

globl_func isr_pendsv

.save_context:
sub sp, sp, #TASK_FRAME_SIZE
strd r4, r5, [sp, #TASK_FRAME_R4]
strd r6, r7, [sp, #TASK_FRAME_R6]
strd r8, r9, [sp, #TASK_FRAME_R8]
strd r10, r11, [sp, #TASK_FRAME_R10]

init_irq_stack
bl HandleTaskSwitch
restore_irq_stack

.restore_context:
ldrd r4, r5, [sp, #TASK_FRAME_R4]
ldrd r6, r7, [sp, #TASK_FRAME_R6]
ldrd r8, r9, [sp, #TASK_FRAME_R8]
ldrd r10, r11, [sp, #TASK_FRAME_R10]
add sp, sp, #TASK_FRAME_SIZE

bx lr
