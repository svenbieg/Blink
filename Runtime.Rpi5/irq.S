//=======
// irq.S
//=======

.section ".text"


//=======
// Using
//=======

#include <cpu.h>
#include <irq.h>


//=============
// IRQ-Handler
//=============

.globl irq_handler
irq_handler:

_save_irq_context:
sub sp, sp, #IRQ_FRAME_SIZE
stp x0, x1, [sp, IRQ_FRAME_X0]
stp x2, x3, [sp, IRQ_FRAME_X2]
stp x4, x5, [sp, IRQ_FRAME_X4]
stp x6, x7, [sp, IRQ_FRAME_X6]
stp x8, x9, [sp, IRQ_FRAME_X8]
stp x10, x11, [sp, IRQ_FRAME_X10]
stp x12, x13, [sp, IRQ_FRAME_X12]
stp x14, x15, [sp, IRQ_FRAME_X14]
stp x16, x17, [sp, IRQ_FRAME_X16]
stp x18, x19, [sp, IRQ_FRAME_X18]
stp x20, x21, [sp, IRQ_FRAME_X20]
stp x22, x23, [sp, IRQ_FRAME_X22]
stp x24, x25, [sp, IRQ_FRAME_X24]
stp x26, x27, [sp, IRQ_FRAME_X26]
stp x28, x29, [sp, IRQ_FRAME_X28]
str x30, [sp, IRQ_FRAME_X30]

_init_irq_stack:
mov x3, sp
cpu_id x0
mov x1, #STACK_SIZE
mul x1, x1, x0
ldr x2, =__stack_end
sub x2, x2, x1
mov sp, x2
sub sp, sp, #IRQ_STACK_SIZE
str x3, [sp, IRQ_STACK_SP]

_handle_interrupt:
bl HandleInterrupt

_restore_irq_stack:
ldr x0, [sp, IRQ_STACK_SP]
mov sp, x0

_restore_irq_context:
ldp x0, x1, [sp, IRQ_FRAME_X0]
ldp x2, x3, [sp, IRQ_FRAME_X2]
ldp x4, x5, [sp, IRQ_FRAME_X4]
ldp x6, x7, [sp, IRQ_FRAME_X6]
ldp x8, x9, [sp, IRQ_FRAME_X8]
ldp x10, x11, [sp, IRQ_FRAME_X10]
ldp x12, x13, [sp, IRQ_FRAME_X12]
ldp x14, x15, [sp, IRQ_FRAME_X14]
ldp x16, x17, [sp, IRQ_FRAME_X16]
ldp x18, x19, [sp, IRQ_FRAME_X18]
ldp x20, x21, [sp, IRQ_FRAME_X20]
ldp x22, x23, [sp, IRQ_FRAME_X22]
ldp x24, x25, [sp, IRQ_FRAME_X24]
ldp x26, x27, [sp, IRQ_FRAME_X26]
ldp x28, x29, [sp, IRQ_FRAME_X28]
ldr x30, [sp, IRQ_FRAME_X30]
add sp, sp, #IRQ_FRAME_SIZE

_finish_irq:
eret
