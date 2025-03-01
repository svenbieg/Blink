//==========
// except.S
//==========

#include "except.h"


//========
// Macros
//========

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
b err_hang

fiq_handler:
mov x0, #FIQ_EXCEPTION
mrs x1, elr_el1
b err_hang

error_handler:
mov x0, #ERROR_EXCEPTION
mrs x1, elr_el1
b err_hang

invalid_handler:
mov x0, #INVALID_EXCEPTION
mrs x1, elr_el1
b err_hang

err_hang:
wfe
b err_hang
