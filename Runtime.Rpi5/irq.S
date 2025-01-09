//=======
// irq.S
//=======

#include <cpu.h>
#include <irq.h>


//=============
// IRQ-Handler
//=============

.globl irq_handler
irq_handler:
msr daifset, #2

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
str x18, [sp, IRQ_FRAME_X18]
stp x29, x30, [sp, IRQ_FRAME_X29]

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
msr daifclr, #2
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
ldr x18, [sp, IRQ_FRAME_X18]
ldp x29, x30, [sp, IRQ_FRAME_X29]
add sp, sp, #IRQ_FRAME_SIZE

_finish_irq:
eret
