//==========
// except.S
//==========

.section ".text"


//=======
// Using
//=======

#include <except.h>


//========
// Common
//========

.globl exc_restore_context
exc_restore_context:
ldp x2, x3, [x0, EXC_FRAME_X2]
ldp x4, x5, [x0, EXC_FRAME_X4]
ldp x6, x7, [x0, EXC_FRAME_X6]
ldp x8, x9, [x0, EXC_FRAME_X8]
ldp x10, x11, [x0, EXC_FRAME_X10]
ldp x12, x13, [x0, EXC_FRAME_X12]
ldp x14, x15, [x0, EXC_FRAME_X14]
ldp x16, x17, [x0, EXC_FRAME_X16]
ldp x18, x19, [x0, EXC_FRAME_X18]
ldp x20, x21, [x0, EXC_FRAME_X20]
ldp x22, x23, [x0, EXC_FRAME_X22]
ldp x24, x25, [x0, EXC_FRAME_X24]
ldp x26, x27, [x0, EXC_FRAME_X26]
ldp x28, x29, [x0, EXC_FRAME_X28]
ldp x30, x1, [x0, EXC_FRAME_X30]
mov sp, x1
ldp x0, x1, [x0, EXC_FRAME_X0]
ret

.globl exc_resume
exc_resume:
ldr x3, [x0, EXC_FRAME_SP]
mov sp, x3
mov x0, x2
br x1

.globl exc_save_context
exc_save_context:
stp x0, x1, [x0, EXC_FRAME_X0]
stp x2, x3, [x0, EXC_FRAME_X2]
stp x4, x5, [x0, EXC_FRAME_X4]
stp x6, x7, [x0, EXC_FRAME_X6]
stp x8, x9, [x0, EXC_FRAME_X8]
stp x10, x11, [x0, EXC_FRAME_X10]
stp x12, x13, [x0, EXC_FRAME_X12]
stp x14, x15, [x0, EXC_FRAME_X14]
stp x16, x17, [x0, EXC_FRAME_X16]
stp x18, x19, [x0, EXC_FRAME_X18]
stp x20, x21, [x0, EXC_FRAME_X20]
stp x22, x23, [x0, EXC_FRAME_X22]
stp x24, x25, [x0, EXC_FRAME_X24]
stp x26, x27, [x0, EXC_FRAME_X26]
stp x28, x29, [x0, EXC_FRAME_X28]
mov x1, sp
stp x30, x1, [x0, EXC_FRAME_X30]
ret
