//==============
// Exceptions.S
//==============

#include "Exceptions.h"
#include "Settings.h"


//========
// Macros
//========

.macro get_cpu_id reg
mrs \reg, MPIDR_EL1
lsr \reg, \reg, #8
and \reg, \reg, #(CPU_COUNT-1)
.endm

.macro ventry label
.align 7
b \label
.endm


//===================
// Exception Vectors
//===================

.align 11
.globl exception_vectors
exception_vectors:
ventry sync_handler // Synchronous EL1T
ventry irq_handler
ventry fiq_handler
ventry error_handler

ventry sync_handler // Synchronous EL1H
ventry irq_handler
ventry fiq_handler
ventry error_handler

ventry invalid_handler // Synchronous 64-bit EL0
ventry invalid_handler
ventry invalid_handler
ventry invalid_handler

ventry invalid_handler // Synchronous 32-bit EL0
ventry invalid_handler
ventry invalid_handler
ventry invalid_handler


//====================
// Exception Handlers
//====================

sync_handler:
mov x0, #SYNC_EXCEPTION
mrs x1, elr_el1
bl HandleException
b err_hang

fiq_handler:
mov x0, #FIQ_EXCEPTION
mrs x1, elr_el1
bl HandleException
b err_hang

error_handler:
mov x0, #ERROR_EXCEPTION
mrs x1, elr_el1
bl HandleException
b err_hang

invalid_handler:
mov x0, #INVALID_EXCEPTION
mrs x1, elr_el1
bl HandleException
b err_hang

err_hang:
wfe
b err_hang


//=============
// IRQ-Handler
//=============

.globl irq_handler
irq_handler:
msr daifset, #2

_save_context:
sub sp, sp, #EXC_FRAME_SIZE
stp x0, x1, [sp, EXC_FRAME_X0]
stp x2, x3, [sp, EXC_FRAME_X2]
stp x4, x5, [sp, EXC_FRAME_X4]
stp x6, x7, [sp, EXC_FRAME_X6]
stp x8, x9, [sp, EXC_FRAME_X8]
stp x10, x11, [sp, EXC_FRAME_X10]
stp x12, x13, [sp, EXC_FRAME_X12]
stp x14, x15, [sp, EXC_FRAME_X14]
stp x16, x17, [sp, EXC_FRAME_X16]
str x18, [sp, EXC_FRAME_X18]
stp x29, x30, [sp, EXC_FRAME_X29]

_init_stack:
mov x3, sp
get_cpu_id x0
mov x1, #STACK_SIZE
mul x1, x1, x0
ldr x2, =__stack_end
sub x2, x2, x1
mov sp, x2
sub sp, sp, #EXC_STACK_SIZE
mov x1, #2
str x1, [sp, EXC_STACK_DAIF]
str x3, [sp, EXC_STACK_SP]

_handle_interrupt:
bl HandleInterrupt

_restore_stack:
ldr x0, [sp, EXC_STACK_DAIF]
cbz x0, 1f
msr daifclr, #2
1:
ldr x0, [sp, EXC_STACK_SP]
mov sp, x0

_restore_context:
ldp x0, x1, [sp, EXC_FRAME_X0]
ldp x2, x3, [sp, EXC_FRAME_X2]
ldp x4, x5, [sp, EXC_FRAME_X4]
ldp x6, x7, [sp, EXC_FRAME_X6]
ldp x8, x9, [sp, EXC_FRAME_X8]
ldp x10, x11, [sp, EXC_FRAME_X10]
ldp x12, x13, [sp, EXC_FRAME_X12]
ldp x14, x15, [sp, EXC_FRAME_X14]
ldp x16, x17, [sp, EXC_FRAME_X16]
ldr x18, [sp, EXC_FRAME_X18]
ldp x29, x30, [sp, EXC_FRAME_X29]
add sp, sp, #EXC_FRAME_SIZE

_finish_irq:
eret
