//========
// task.S
//========

#include <irq.h>
#include <task.h>


//==============
// Task-Context
//==============

.globl task_init
task_init:
ldr x3, [x0]
sub x3, x3, IRQ_FRAME_SIZE
str x2, [x3, IRQ_FRAME_X0]
sub x3, x3, TASK_FRAME_SIZE
str x1, [x3, TASK_FRAME_ELR]
str x3, [x0]
ret

.globl task_restore_context
task_restore_context:
ldr x1, [x0, TASK_FRAME_ELR]
msr elr_el1, x1
ldp x19, x20, [x0, TASK_FRAME_X19]
ldp x21, x22, [x0, TASK_FRAME_X21]
ldp x23, x24, [x0, TASK_FRAME_X23]
ldp x25, x26, [x0, TASK_FRAME_X25]
ldp x27, x28, [x0, TASK_FRAME_X27]
ldp x1, x2, [x0, TASK_FRAME_FPCR]
msr fpcr, x1
msr fpsr, x2
ldp d8, d9, [x0, TASK_FRAME_D8]
ldp d10, d11, [x0, TASK_FRAME_D10]
ldp d12, d13, [x0, TASK_FRAME_D12]
ldp d14, d15, [x0, TASK_FRAME_D14]
add x0, x0, TASK_FRAME_SIZE
ret

.globl task_save_context
task_save_context:
sub x0, x0, TASK_FRAME_SIZE
mrs x1, elr_el1
str x1, [x0, TASK_FRAME_ELR]
stp x19, x20, [x0, TASK_FRAME_X19]
stp x21, x22, [x0, TASK_FRAME_X21]
stp x23, x24, [x0, TASK_FRAME_X23]
stp x25, x26, [x0, TASK_FRAME_X25]
stp x27, x28, [x0, TASK_FRAME_X27]
mrs x1, fpcr
mrs x2, fpsr
stp x1, x2, [x0, TASK_FRAME_FPCR]
stp d8, d9, [x0, TASK_FRAME_D8]
stp d10, d11, [x0, TASK_FRAME_D10]
stp d12, d13, [x0, TASK_FRAME_D12]
stp d14, d15, [x0, TASK_FRAME_D14]
ret
